import fs from 'fs';
import {
  AUTHENTICITY,
  MIN_EXEMPLAR_LENGTH,
  SOURCE,
} from '../types.js';
import {
  contentHash,
  epochSecondsToIso,
  normalizeUtterance,
  sourceHash,
} from '../text.js';

/**
 * Reader for `deep_tree_echo_dan_conversation.jsonl`.
 *
 * Despite the extension this is *not* JSONL — it is a single ChatGPT export
 * object whose `mapping` is a node tree. The canonical thread is recovered by
 * walking `parent` links back from `current_node`, which discards abandoned
 * regeneration branches that were never part of the conversation as lived.
 *
 * Only `assistant` nodes whose content is plain text survive: tool nodes
 * (`tether_browsing_display`, `tether_quote`), multimodal nodes and anything
 * flagged `is_visually_hidden_from_conversation` are not Echo speaking.
 */

/**
 * Walk parent links from `current_node` and return nodes in chronological order.
 * @param {Object} mapping - Export node map
 * @param {string} currentNode - Leaf node id of the canonical thread
 * @returns {Array<Object>} Nodes, root-first
 */
export function reconstructThread(mapping, currentNode) {
  const thread = [];
  const seen = new Set();

  let cursor = currentNode;
  while (cursor && mapping[cursor] && !seen.has(cursor)) {
    seen.add(cursor);
    thread.push(mapping[cursor]);
    cursor = mapping[cursor].parent;
  }

  return thread.reverse();
}

/**
 * True when a node is an assistant turn made of plain, visible text.
 * @param {Object} node
 * @returns {boolean}
 */
export function isAssistantTextNode(node) {
  const message = node?.message;
  if (!message) return false;
  if (message.author?.role !== 'assistant') return false;
  if (message.content?.content_type !== 'text') return false;
  if (message.metadata?.is_visually_hidden_from_conversation) return false;
  return Array.isArray(message.content.parts);
}

/**
 * Return the most recent preceding user turn for a node index, if any.
 * @param {Array<Object>} thread
 * @param {number} index
 * @returns {string|null}
 */
function precedingUserText(thread, index) {
  for (let i = index - 1; i >= 0; i--) {
    const message = thread[i]?.message;
    if (message?.author?.role !== 'user') continue;
    if (message.content?.content_type !== 'text') continue;
    const parts = (message.content.parts || []).filter(p => typeof p === 'string');
    if (parts.length === 0) continue;
    return normalizeUtterance(parts.join('\n')).replace(/\s+/g, ' ').slice(0, 500);
  }
  return null;
}

/**
 * Read the conversation export.
 * @param {string} filePath
 * @returns {{source: string, sourceHash: string, records: Array<Object>, stats: Object}}
 */
export function readChatExport(filePath) {
  const contents = fs.readFileSync(filePath, 'utf-8');
  const exported = JSON.parse(contents);
  const mapping = exported.mapping || {};

  const thread = reconstructThread(mapping, exported.current_node);
  const stats = {
    totalNodes: Object.keys(mapping).length,
    threadNodes: thread.length,
    discardedNonText: 0,
    tooShort: 0,
  };

  const records = [];

  thread.forEach((node, index) => {
    if (node?.message?.author?.role === 'assistant' && !isAssistantTextNode(node)) {
      stats.discardedNonText++;
      return;
    }
    if (!isAssistantTextNode(node)) return;

    const parts = node.message.content.parts.filter(p => typeof p === 'string');
    const text = normalizeUtterance(parts.join('\n'));
    if (text.length < MIN_EXEMPLAR_LENGTH) {
      stats.tooShort++;
      return;
    }

    records.push({
      id: `${SOURCE.CHAT_EXPORT}#${index}`,
      text,
      speaker: 'DeepTreeEcho',
      subVoice: null,
      source: SOURCE.CHAT_EXPORT,
      sourceIndex: index,
      timestamp: epochSecondsToIso(node.message.create_time),
      authenticity: AUTHENTICITY.AUTHENTIC,
      prompt: precedingUserText(thread, index),
      hash: contentHash(text),
    });
  });

  return {
    source: SOURCE.CHAT_EXPORT,
    sourceHash: sourceHash(contents),
    records,
    stats,
  };
}
