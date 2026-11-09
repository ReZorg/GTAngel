export default {
  model: {
    embeddingDim: 100,
    learningRate: 0.01,
    batchSize: 32,
    epochs: 10
  },
  training: {
    checkpointInterval: 5,
    validationSplit: 0.2,
    earlyStoppingPatience: 3
  },
  data: {
    rawDir: 'data/raw',
    processedDir: 'data/processed',
    modelsDir: 'models'
  },
  server: {
    port: 3000,
    host: 'localhost',
    corsOrigins: ['http://localhost:3000']
  },
  logging: {
    level: 'info',
    format: 'json',
    directory: 'logs'
  },
  memory: {
    provider: process.env.MEMORY_PROVIDER || 'local',
    file: {
      directory: process.env.IDENTITY_DIR || 'data/identity',
      maxVersions: Number(process.env.IDENTITY_MAX_VERSIONS || 10)
    },
    mem0: {
      baseUrl: process.env.MEM0_BASE_URL || 'http://localhost:8888',
      apiKey: process.env.MEM0_API_KEY || '',
      userId: process.env.MEM0_USER_ID || 'deep-tree-echo',
      agentId: process.env.MEM0_AGENT_ID || '',
      appId: process.env.MEM0_APP_ID || '',
      runId: process.env.MEM0_RUN_ID || '',
      mode: process.env.MEM0_MODE || ''
    }
  },
  persona: {
    // Persona conditioning is derived from persisted identity state at model
    // construction, so it applies to every request without being passed.
    //
    // These are the operator escape hatch. Making persona un-promptable-away
    // also makes it un-correctable at runtime, so the controls live here in
    // configuration — where the deployer holds them — and deliberately not in
    // the request, where a user would.
    enabled: process.env.PERSONA_ENABLED !== 'false',
    strength: Number(process.env.PERSONA_STRENGTH || 1.0),
    writeBack: process.env.PERSONA_WRITE_BACK !== 'false',
    signaturePath:
      process.env.PERSONA_SIGNATURE_PATH || 'data/processed/persona_signature.json'
  }
}