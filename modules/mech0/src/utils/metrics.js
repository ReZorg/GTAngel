import { encodeTokens } from './tokenizer.js';

export function calculateAccuracy(predictions, actual) {
  if (!predictions || !actual || predictions.length === 0 || predictions.length !== actual.length) {
    throw new Error('Invalid input: predictions and actual must be non-empty arrays of equal length');
  }
  const correct = predictions.filter((pred, i) => pred === actual[i]).length;
  return correct / predictions.length;
}

export function calculatePrecision(predictions, actual, label) {
  const truePositives = predictions.filter((pred, i) =>
    pred === label && actual[i] === label
  ).length;

  const predictedPositives = predictions.filter(pred =>
    pred === label
  ).length;

  return predictedPositives === 0 ? 0 : truePositives / predictedPositives;
}

export function calculateRecall(predictions, actual, label) {
  const truePositives = predictions.filter((pred, i) =>
    pred === label && actual[i] === label
  ).length;

  // Fix: was `actual.filter(label => label === label)` — shadowed parameter always true
  const actualPositives = actual.filter(a =>
    a === label
  ).length;

  return actualPositives === 0 ? 0 : truePositives / actualPositives;
}
