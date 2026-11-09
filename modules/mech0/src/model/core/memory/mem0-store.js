import { MemoryStore } from './store.js';
import logger from '../../../utils/logger.js';

const DEFAULT_MEM0_USER_ID = 'deep-tree-echo';

export class Mem0MemoryStore extends MemoryStore {
  constructor(config = {}) {
    super(config);
    const memoryConfig = config.memory || {};
    const mem0Config = memoryConfig.mem0 || config.mem0 || {};

    this.provider = 'mem0';
    this.baseUrl = (mem0Config.baseUrl || 'http://localhost:8888').replace(/\/+$/, '');
    this.apiKey = mem0Config.apiKey || null;
    this.mode = mem0Config.mode || this.inferMode(this.baseUrl);
    this.fetch = mem0Config.fetch || globalThis.fetch;
    this.scope = {
      user_id: mem0Config.userId || DEFAULT_MEM0_USER_ID,
      agent_id: mem0Config.agentId,
      app_id: mem0Config.appId,
      run_id: mem0Config.runId
    };
    this.enabled = Boolean(this.fetch && this.apiKey?.trim());
  }

  async store(key, memory, context = {}) {
    const entry = await super.store(key, memory, context);
    await this.persistSnapshot(key, entry);
    return entry;
  }

  async recall(key, context = {}) {
    const localEntry = await super.recall(key, context);
    if (localEntry || !this.enabled) {
      return localEntry;
    }

    const remoteMemory = await this.fetchRemoteMemory(key);
    if (!remoteMemory) {
      return null;
    }

    const hydratedEntry = await super.store(
      key,
      this.extractContent(remoteMemory),
      this.extractContext(remoteMemory)
    );

    hydratedEntry.timestamp = remoteMemory.updated_at ||
      remoteMemory.created_at ||
      hydratedEntry.timestamp;
    hydratedEntry.accessCount = remoteMemory.metadata?.accessCount || hydratedEntry.accessCount;

    return super.recall(key, context);
  }

  async associate(key, tags) {
    const associated = await super.associate(key, tags);
    if (!associated) {
      return false;
    }

    const entry = this.getEntry(key);
    if (entry) {
      await this.persistSnapshot(key, entry);
    }

    return true;
  }

  async findSimilar(key, limit = 5) {
    const localMatches = await super.findSimilar(key, limit);
    if (localMatches.length > 0 || !this.enabled) {
      return localMatches;
    }

    const entry = this.getEntry(key) || await this.recall(key);
    if (!entry) {
      return [];
    }

    try {
      const remoteMemories = await this.listRemoteMemories();
      const requestedTags = new Set(entry.associations);

      const related = [];
      for (const remoteMemory of remoteMemories) {
        const remoteKey = this.extractKey(remoteMemory);
        if (!remoteKey || remoteKey === key) {
          continue;
        }

        const remoteContext = this.extractContext(remoteMemory);
        const remoteTags = new Set(remoteContext.tags || []);
        const sharedTags = Array.from(requestedTags)
          .filter(tag => remoteTags.has(tag)).length;

        if (sharedTags === 0) {
          continue;
        }

        const hydrated = this.getEntry(remoteKey) || await super.store(
          remoteKey,
          this.extractContent(remoteMemory),
          remoteContext
        );

        related.push({
          key: remoteKey,
          memory: hydrated,
          strength: sharedTags / Math.max(requestedTags.size, remoteTags.size, 1)
        });
      }

      return related
        .sort((a, b) => b.strength - a.strength)
        .slice(0, limit);
    } catch (error) {
      logger.warn('Failed to fetch related memories from mem0, using in-memory results only', {
        error: error.message
      });
      return localMatches;
    }
  }

  inferMode(baseUrl) {
    return baseUrl.includes('api.mem0.ai') ? 'platform' : 'oss';
  }

  getHeaders() {
    const headers = {
      'Content-Type': 'application/json'
    };

    if (this.mode === 'platform') {
      headers.Authorization = `Token ${this.apiKey}`;
    } else {
      headers['X-API-Key'] = this.apiKey;
    }

    return headers;
  }

  buildScope() {
    return Object.fromEntries(
      Object.entries(this.scope).filter(([, value]) => value)
    );
  }

  buildMetadata(key, entry) {
    return {
      key,
      source: 'echo',
      accessCount: entry.accessCount,
      version: entry.version,
      originalContent: entry.content,
      context: {
        ...entry.context,
        tags: Array.from(entry.associations)
      }
    };
  }

  getEntry(key) {
    return this.shortTerm.get(key) || this.longTerm.get(key) || null;
  }

  serializeContent(content) {
    if (typeof content === 'string') {
      return content;
    }

    if (typeof content?.text === 'string') {
      return content.text;
    }

    return JSON.stringify(content);
  }

  formatRemoteMemory(key, entry) {
    return `[echo-memory:${key}] ${this.serializeContent(entry.content)}`;
  }

  extractKey(memory) {
    if (memory?.metadata?.key) {
      return memory.metadata.key;
    }

    const match = typeof memory?.memory === 'string'
      ? memory.memory.match(/^\[echo-memory:([^\]]+)\]\s*/)
      : null;

    return match?.[1] || null;
  }

  extractContext(memory) {
    const context = memory?.metadata?.context || {};
    const tags = context.tags || memory?.metadata?.tags || [];

    return {
      ...context,
      tags
    };
  }

  extractContent(memory) {
    if (memory?.metadata?.originalContent !== undefined) {
      return memory.metadata.originalContent;
    }

    if (typeof memory?.memory !== 'string') {
      return memory?.memory;
    }

    return memory.memory.replace(/^\[echo-memory:[^\]]+\]\s*/, '');
  }

  normalizeMemoryList(payload) {
    if (Array.isArray(payload)) {
      return payload;
    }

    if (Array.isArray(payload?.results)) {
      return payload.results;
    }

    if (Array.isArray(payload?.memories)) {
      return payload.memories;
    }

    if (Array.isArray(payload?.data?.results)) {
      return payload.data.results;
    }

    return [];
  }

  async persistSnapshot(key, entry) {
    if (!this.enabled) {
      return;
    }

    try {
      const body = this.mode === 'platform'
        ? {
            ...this.buildScope(),
            memory: this.formatRemoteMemory(key, entry),
            metadata: this.buildMetadata(key, entry)
          }
        : {
            ...this.buildScope(),
            messages: [
              {
                role: 'user',
                content: this.formatRemoteMemory(key, entry)
              }
            ],
            metadata: this.buildMetadata(key, entry)
          };

      const endpoint = this.mode === 'platform'
        ? `${this.baseUrl}/v3/memories/add/`
        : `${this.baseUrl}/memories`;

      const response = await this.fetch(endpoint, {
        method: 'POST',
        headers: this.getHeaders(),
        body: JSON.stringify(body)
      });

      if (!response.ok) {
        throw new Error(`mem0 store failed with status ${response.status}`);
      }
    } catch (error) {
      logger.warn('Failed to persist memory snapshot to mem0, keeping in-memory copy', {
        error: error.message,
        key
      });
    }
  }

  async listRemoteMemories() {
    const scope = this.buildScope();
    const endpoint = this.mode === 'platform'
      ? `${this.baseUrl}/v3/memories/`
      : `${this.baseUrl}/memories?${new URLSearchParams(scope).toString()}`;
    const request = this.mode === 'platform'
      ? {
          method: 'POST',
          headers: this.getHeaders(),
          body: JSON.stringify({
            ...scope,
            page_size: 200
          })
        }
      : {
          method: 'GET',
          headers: this.getHeaders()
        };

    const response = await this.fetch(endpoint, request);
    if (!response.ok) {
      throw new Error(`mem0 list failed with status ${response.status}`);
    }

    return this.normalizeMemoryList(await response.json());
  }

  async fetchRemoteMemory(key) {
    if (!this.enabled) {
      return null;
    }

    try {
      const memories = await this.listRemoteMemories();
      return memories
        .filter(memory => this.extractKey(memory) === key)
        .sort((left, right) => {
          const leftTime = new Date(left.updated_at || left.created_at || 0).getTime();
          const rightTime = new Date(right.updated_at || right.created_at || 0).getTime();
          return rightTime - leftTime;
        })[0] || null;
    } catch (error) {
      logger.warn('Failed to restore memory from mem0, using in-memory results only', {
        error: error.message,
        key
      });
      return null;
    }
  }
}
