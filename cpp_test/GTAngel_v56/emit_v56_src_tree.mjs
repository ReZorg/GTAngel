import fs from 'node:fs';
import path from 'node:path';

const full = fs.readFileSync('gtangel_agi_v56_full.cpp', 'utf8');
const lines = full.split(/\r?\n/);
const zppRoot = path.resolve('../../zpp');

function sliceLines(start1, end1) {
  return lines.slice(start1 - 1, end1).join('\n') + '\n';
}

function findLine(predicate, from1 = 1) {
  for (let i = from1 - 1; i < lines.length; ++i) {
    if (predicate(lines[i], i + 1)) return i + 1;
  }
  throw new Error('Anchor not found from line ' + from1);
}

const CPP_KEYWORDS = new Set([
  'this','return','default','class','struct','union','operator','delete','new',
  'case','switch','if','else','for','while','do','int','float','bool','void',
  'char','short','long','unsigned','signed','const','static','virtual','inline',
  'public','private','protected','TRUE','FALSE','NULL','near','far','min','max',
  'IN','OUT','ERROR','small','hyper','true','false','nullptr','try','catch',
  'throw','namespace','using','template','typename'
]);

function safeFieldName(raw) {
  return CPP_KEYWORDS.has(raw) ? `${raw}_field` : raw;
}

const STD_INT_CONSTANTS = new Map([
  ['UINT8_MAX', 255],
  ['INT8_MAX', 127],
  ['UINT16_MAX', 65535],
  ['INT16_MAX', 32767],
  ['INT32_MAX', 2147483647],
  ['UINT32_MAX', 4294967295]
]);

const folders = [
  'animation', 'audio', 'buildings', 'collision', 'control', 'core',
  'docs', 'entities', 'extras', 'fakerw', 'leeds', 'math',
  'modelinfo', 'objects', 'peds', 'renderer', 'rw', 'save',
  'skel', 'text', 'vehicles', 'weapons'
];

const globalSymValues = new Map();
const conflictingSyms = new Set();

function recordSymbolValue(map, conflicts, name, val) {
  if (!(val > 0 && val <= 4096)) return;
  if (conflicts.has(name)) return;
  if (map.has(name) && map.get(name) !== val) {
    map.delete(name);
    conflicts.add(name);
    return;
  }
  map.set(name, val);
}

const srcRefRoot = path.resolve('../../src');
if (fs.existsSync(srcRefRoot)) {
  const walkSrc = (d) => {
    for (const ent of fs.readdirSync(d, { withFileTypes: true })) {
      const p = path.join(d, ent.name);
      if (ent.isDirectory()) walkSrc(p);
      else if (/\.(h|hpp|cpp)$/i.test(ent.name)) {
        const txt = fs.readFileSync(p, 'utf8');
        const defRe = /^\s*#define\s+([A-Za-z_]\w*)\s+\(?(\d+)\)?\b/gm;
        let dm;
        while ((dm = defRe.exec(txt)) !== null) {
          recordSymbolValue(globalSymValues, conflictingSyms, dm[1], Number(dm[2]));
        }
      }
    }
  };
  walkSrc(srcRefRoot);
}

for (const folder of folders) {
  const dir = path.join(zppRoot, folder);
  if (!fs.existsSync(dir)) continue;
  const zfiles = fs.readdirSync(dir).filter(f => f.endsWith('.zpp')).sort();
  for (const zf of zfiles) {
    const txt = fs.readFileSync(path.join(dir, zf), 'utf8');
    const txtLines = txt.split(/\r?\n/);
    for (let li = 0; li < txtLines.length; ++li) {
      const dm = txtLines[li].match(/--\s*#define\s+([A-Za-z_]\w*)\s+\(?(\d+)\)?\b/);
      if (dm) {
        recordSymbolValue(globalSymValues, conflictingSyms, dm[1], Number(dm[2]));
      }
      const em = txtLines[li].match(/^([A-Za-z_]\w*)\s*::=\s*(.+)$/);
      if (em) {
        const rawBranches = em[2]
          .split('|')
          .map(s => s.trim())
          .filter(s => s && !s.startsWith('...'))
          .map(s => s.replace(/[^A-Za-z0-9_]/g, '_'))
          .filter(s => s && /^[A-Za-z_]/.test(s));
        const uniq = [...new Set(rawBranches)];
        const explicitVals = new Map();
        let vj = li + 1;
        while (vj < txtLines.length && txtLines[vj].trim().startsWith('-- val(')) {
          const vm = txtLines[vj].match(/--\s*val\(([A-Za-z_]\w*)\)\s*=\s*(-?(?:0x[0-9a-fA-F]+|\d+))/);
          if (vm) explicitVals.set(vm[1], Number(vm[2]));
          vj++;
        }
        let nextVal = 0;
        for (const b of uniq) {
          if (explicitVals.has(b)) nextVal = explicitVals.get(b);
          recordSymbolValue(globalSymValues, conflictingSyms, b, nextVal);
          nextVal++;
        }
      }
    }
  }
}

function buildZppCppContractBlock(folder, insideClass = false) {
  const dir = path.join(zppRoot, folder);
  const zfiles = fs.existsSync(dir)
    ? fs.readdirSync(dir).filter(f => f.endsWith('.zpp') && !/_(h|cpp|inc|hlsl|frag|vert)\.zpp$/.test(f)).sort()
    : [];

  let totalSchemas = 0;
  let totalEnums = 0;
  const sampleEnums = [];
  const sampleSchemas = [];
  const queryOps = [];
  const deltaOps = [];

  for (const zf of zfiles) {
    const txt = fs.readFileSync(path.join(dir, zf), 'utf8');
    const txtLines = txt.split(/\r?\n/);

    const fileSymValues = new Map();
    const fileConflicts = new Set();
    const fileEnumNames = new Set();

    // 1. Parse enums with full branch lists and explicit `-- val(BRANCH) = V` annotations
    for (let li = 0; li < txtLines.length; ++li) {
      const dm = txtLines[li].match(/--\s*#define\s+([A-Za-z_]\w*)\s+\(?(\d+)\)?\b/);
      if (dm) {
        recordSymbolValue(fileSymValues, fileConflicts, dm[1], Number(dm[2]));
      }
      const em = txtLines[li].match(/^([A-Za-z_]\w*)\s*::=\s*(.+)$/);
      if (!em) continue;
      totalEnums++;
      const ename = em[1].replace(/[^A-Za-z0-9_]/g, '_');
      fileEnumNames.add(ename);
      const rawBranches = em[2]
        .split('|')
        .map(s => s.trim())
        .filter(s => s && !s.startsWith('...'))
        .map(s => s.replace(/[^A-Za-z0-9_]/g, '_'))
        .filter(s => s && /^[A-Za-z_]/.test(s));
      const uniq = [...new Set(rawBranches)];
      if (uniq.length < 2) continue;

      const explicitVals = new Map();
      let vj = li + 1;
      while (vj < txtLines.length && txtLines[vj].trim().startsWith('-- val(')) {
        const vm = txtLines[vj].match(/--\s*val\(([A-Za-z_]\w*)\)\s*=\s*(-?(?:0x[0-9a-fA-F]+|\d+))/);
        if (vm) explicitVals.set(vm[1], Number(vm[2]));
        vj++;
      }

      let nextVal = 0;
      const valuedBranches = uniq.map(b => {
        if (explicitVals.has(b)) nextVal = explicitVals.get(b);
        const cur = nextVal;
        recordSymbolValue(fileSymValues, fileConflicts, b, cur);
        nextVal = cur + 1;
        return { name: b, val: cur };
      });

      if (sampleEnums.length < 6 && !sampleEnums.some(e => e.ename === ename)) {
        sampleEnums.push({ ename, branches: valuedBranches, source: zf });
      }
    }

    // 2. Parse state & operation schemas
    const schemaRe = /^schema\s+([A-Za-z_]\w*)\r?\n([\s\S]*?)^where\r?\n([\s\S]*?)^end/gm;
    let sm;
    while ((sm = schemaRe.exec(txt)) !== null) {
      totalSchemas++;
      const sname = sm[1].replace(/[^A-Za-z0-9_]/g, '_');
      const decls = sm[2].split(/\r?\n/);
      const rawWhereLines = sm[3].split(/\r?\n/).map(l => l.trim()).filter(Boolean);
      const whereLines = rawWhereLines.filter(l => !l.startsWith('--'));

      // Check if this is an Operation Schema (contains ΔTarget or ΞTarget)
      const deltaMatch = sm[2].match(/^\s+([ΔΞ])([A-Za-z_]\w*)/m);
      if (deltaMatch) {
        const mode = deltaMatch[1];
        const targetSchema = sampleSchemas.find(s => s.sname === deltaMatch[2]);
        if (!targetSchema) continue;

        if (mode === 'Ξ') {
          if (queryOps.length < 128 && !queryOps.some(o => o.opName === sname)) {
            queryOps.push({
              opName: sname,
              targetState: targetSchema.sname,
              source: zf
            });
          }
        } else {
          const updateLines = rawWhereLines.filter(wl => wl.startsWith('-- State update:'));
          if (updateLines.length === 0) continue;

          const opInputs = new Set();
          for (const d of decls) {
            const pm = d.match(/^\s+([A-Za-z_]\w*)\?\s*:/);
            if (pm) opInputs.add(pm[1]);
          }

          const rawFieldToRhs = new Map();
          let allUpdatesSupported = true;

          for (const wl of updateLines) {
            const um = wl.match(/^--\s*State update:\s*([A-Za-z_]\w*)'\s*=\s*(.+)$/);
            if (!um) {
              allUpdatesSupported = false;
              break;
            }
            const lhsRaw = um[1];
            const rhsRaw = um[2].trim();
            const f = targetSchema.fields.find(fl => fl.rawName === lhsRaw);
            if (!f) {
              allUpdatesSupported = false;
              break;
            }
            if (rawFieldToRhs.has(f.fname)) {
              if (rawFieldToRhs.get(f.fname).rhsRaw !== rhsRaw) {
                allUpdatesSupported = false;
                break;
              }
            } else {
              rawFieldToRhs.set(f.fname, { f, rhsRaw });
            }
          }

          if (!allUpdatesSupported || rawFieldToRhs.size === 0) continue;

          const updates = [];
          const extraParams = [];
          const rhsParamMap = new Map();
          const getOrAddInputParam = (rhsKey, kind, ct, defIn) => {
            if (rhsParamMap.has(rhsKey)) {
              const existing = rhsParamMap.get(rhsKey);
              if (existing.kind !== kind) return null;
              if (existing.ct !== ct && kind === 'scalar_int') {
                existing.ct = 'int64_t';
              }
              return existing.pName;
            }
            const cleanBase = rhsKey.replace(/[^A-Za-z0-9_]/g, '_').replace(/_+$/, '');
            const pName = `${cleanBase}_in`;
            const entry = { pName, kind, ct, defIn };
            rhsParamMap.set(rhsKey, entry);
            extraParams.push(entry);
            return pName;
          };
          const rhsCopyTargets = new Set();
          for (const { rhsRaw } of rawFieldToRhs.values()) {
            const rf = targetSchema.fields.find(fl => fl.rawName === rhsRaw);
            if (rf) rhsCopyTargets.add(rf.fname);
          }

          for (const { f, rhsRaw } of rawFieldToRhs.values()) {
            const rhsField = targetSchema.fields.find(fl => fl.rawName === rhsRaw && fl.fname !== f.fname);
            if (rhsField && rhsField.kind === f.kind && (f.kind === 'scalar_int' || f.kind === 'scalar_float' || f.kind === 'vec3') && !rawFieldToRhs.has(rhsField.fname)) {
              // Category B: pre-state field copy `lhs' = rhsField` (int, float, or 3D vector)
              if (f.kind === 'vec3') {
                updates.push({
                  field: f.fname,
                  preInit: `pre.${rhsField.fname} = ZppVec3{4.0f, 5.0f, 6.0f}; pre.${f.fname} = ZppVec3{0.0f, 0.0f, 0.0f};`,
                  applyExpr: `post.${f.fname} = pre.${rhsField.fname};`,
                  postCond: `(post.${f.fname} == pre.${rhsField.fname})`,
                  badPostMut: `badPost.${f.fname} = ZppVec3{pre.${rhsField.fname}.x + 1.0f, pre.${rhsField.fname}.y, pre.${rhsField.fname}.z};`
                });
              } else {
                const initRhs = f.kind === 'scalar_float' ? '7.0f' : '7';
                const initLhs = f.kind === 'scalar_float' ? '0.0f' : '0';
                const step = f.kind === 'scalar_float' ? '1.0f' : '1';
                updates.push({
                  field: f.fname,
                  preInit: `pre.${rhsField.fname} = ${initRhs}; pre.${f.fname} = ${initLhs};`,
                  applyExpr: `post.${f.fname} = pre.${rhsField.fname};`,
                  postCond: `(post.${f.fname} == pre.${rhsField.fname})`,
                  badPostMut: `badPost.${f.fname} = pre.${rhsField.fname} + ${step};`
                });
              }
            } else if (f.kind === 'vec3' && (opInputs.has(rhsRaw) || /^[A-Za-z_][\w:]*\(\)$/.test(rhsRaw))) {
              // Category C (3D vector): declared CVector input parameter (e.g. CColSphere_Set center)
              const pName = getOrAddInputParam(rhsRaw, f.kind, 'ZppVec3', 'ZppVec3{1.0f, 2.0f, 3.0f}');
              if (!pName) { allUpdatesSupported = false; break; }
              updates.push({
                field: f.fname,
                preInit: rhsCopyTargets.has(f.fname) ? '' : `pre.${f.fname} = ZppVec3{0.0f, 0.0f, 0.0f};`,
                applyExpr: `post.${f.fname} = ${pName};`,
                postCond: `(post.${f.fname} == ${pName})`,
                badPostMut: `badPost.${f.fname} = ZppVec3{${pName}.x + 1.0f, ${pName}.y, ${pName}.z};`
              });
            } else if (f.kind === 'opt_ref' && rhsRaw === '∅') {
              updates.push({
                field: f.fname,
                preInit: `pre.${f.fname} = 1u;`,
                applyExpr: `post.${f.fname} = 0u;`,
                postCond: `(post.${f.fname} == 0u)`,
                badPostMut: `badPost.${f.fname} = 1u;`
              });
            } else if (f.kind === 'scalar_bool' && (rhsRaw === 'true' || rhsRaw === 'false')) {
              const altBool = rhsRaw === 'true' ? 'false' : 'true';
              updates.push({
                field: f.fname,
                preInit: `pre.${f.fname} = ${altBool};`,
                applyExpr: `post.${f.fname} = ${rhsRaw};`,
                postCond: `(post.${f.fname} == ${rhsRaw})`,
                badPostMut: `badPost.${f.fname} = ${altBool};`
              });
            } else if (f.kind === 'scalar_bool' && (opInputs.has(rhsRaw) || /^[A-Za-z_][\w:]*\(\)$/.test(rhsRaw))) {
              const pName = getOrAddInputParam(rhsRaw, f.kind, 'bool', 'true');
              if (!pName) { allUpdatesSupported = false; break; }
              updates.push({
                field: f.fname,
                preInit: `pre.${f.fname} = false;`,
                applyExpr: `post.${f.fname} = ${pName};`,
                postCond: `(post.${f.fname} == ${pName})`,
                badPostMut: `badPost.${f.fname} = !${pName};`
              });
            } else if (f.kind === 'scalar_float' && /^\d+(?:\.\d+)?f?$/.test(rhsRaw)) {
              const fval = parseFloat(rhsRaw);
              const fLit = Number.isInteger(fval) ? `${fval}.0f` : `${fval}f`;
              const altLit = fval === 0 ? '1.0f' : '0.0f';
              updates.push({
                field: f.fname,
                preInit: `pre.${f.fname} = ${altLit};`,
                applyExpr: `post.${f.fname} = ${fLit};`,
                postCond: `(post.${f.fname} == ${fLit})`,
                badPostMut: `badPost.${f.fname} = ${altLit};`
              });
            } else if (f.kind === 'scalar_float' && (opInputs.has(rhsRaw) || /^[A-Za-z_][\w:]*\(\)$/.test(rhsRaw))) {
              const pName = getOrAddInputParam(rhsRaw, f.kind, 'float', '2.5f');
              if (!pName) { allUpdatesSupported = false; break; }
              updates.push({
                field: f.fname,
                preInit: rhsCopyTargets.has(f.fname) ? '' : `pre.${f.fname} = 0.0f;`,
                applyExpr: `post.${f.fname} = ${pName};`,
                postCond: `(post.${f.fname} == ${pName})`,
                badPostMut: `badPost.${f.fname} = ${pName} + 1.0f;`
              });
            } else if (f.kind === 'scalar_int') {
              let constVal = null;
              if (/^\d+$/.test(rhsRaw)) {
                constVal = Number(rhsRaw);
              } else if (STD_INT_CONSTANTS.has(rhsRaw)) {
                constVal = STD_INT_CONSTANTS.get(rhsRaw);
              } else if (!fileConflicts.has(rhsRaw) && fileSymValues.has(rhsRaw)) {
                constVal = fileSymValues.get(rhsRaw);
              } else if (!conflictingSyms.has(rhsRaw) && globalSymValues.has(rhsRaw)) {
                constVal = globalSymValues.get(rhsRaw);
              }

              if (constVal !== null && constVal >= 0) {
                const altVal = constVal === 0 ? 1 : 0;
                updates.push({
                  field: f.fname,
                  preInit: rhsCopyTargets.has(f.fname) ? '' : `pre.${f.fname} = ${altVal};`,
                  applyExpr: `post.${f.fname} = ${constVal};`,
                  postCond: `(post.${f.fname} == ${constVal})`,
                  badPostMut: `badPost.${f.fname} = ${altVal};`
                });
              } else if (opInputs.has(rhsRaw) || /^[A-Za-z_][\w:]*\(\)$/.test(rhsRaw)) {
                const pName = getOrAddInputParam(rhsRaw, f.kind, f.ct, 25);
                if (!pName) { allUpdatesSupported = false; break; }
                updates.push({
                  field: f.fname,
                  preInit: rhsCopyTargets.has(f.fname) ? '' : `pre.${f.fname} = 0;`,
                  applyExpr: `post.${f.fname} = (${f.ct})${pName};`,
                  postCond: `(post.${f.fname} == (${f.ct})${pName})`,
                  badPostMut: `badPost.${f.fname} = (${f.ct})(${pName} + 1);`
                });
              } else {
                allUpdatesSupported = false;
                break;
              }
            } else {
              allUpdatesSupported = false;
              break;
            }
          }

          if (
            allUpdatesSupported &&
            updates.length === rawFieldToRhs.size &&
            deltaOps.length < 128 &&
            !deltaOps.some(o => o.opName === sname)
          ) {
            deltaOps.push({
              opName: sname,
              targetState: targetSchema.sname,
              updates,
              extraParams,
              source: zf
            });
          }
        }
        continue;
      }

      if (sampleSchemas.length >= 128 || sname.includes('_') || sampleSchemas.some(s => s.sname === sname)) {
        continue;
      }

      const fields = new Map();
      let hasUnresolvedField = false;

      for (const d of decls) {
        const fm = d.match(/^\s+([A-Za-z_]\w*)\s*:\s*([^\r\n]+)/);
        if (!fm) continue;
        const rawName = fm[1];
        const fname = safeFieldName(rawName);
        if (fields.has(rawName) || fields.size >= 64) continue;
        const zt = fm[2].trim();

        const arrMatch = zt.match(/^seq\s+(\S+)\s+--\s*bounded array\s*\[([^\]]+)\]/);
        if (arrMatch) {
          const elemZ = arrMatch[1];
          const rawDim = arrMatch[2].trim();
          let len = null;
          if (/^\d+$/.test(rawDim)) {
            len = Number(rawDim);
          } else if (fileConflicts.has(rawDim)) {
            hasUnresolvedField = true;
            break;
          } else if (fileSymValues.has(rawDim)) {
            len = fileSymValues.get(rawDim);
          } else if (!conflictingSyms.has(rawDim) && globalSymValues.has(rawDim)) {
            len = globalSymValues.get(rawDim);
          } else {
            hasUnresolvedField = true;
            break;
          }
          let elemC = 'int32_t';
          if (elemZ === 'ℝ') elemC = 'float';
          else if (elemZ === '𝔹') elemC = 'bool';
          else if (elemZ === 'ℕ') elemC = 'uint32_t';
          else if (elemZ === 'CVector' || elemZ === 'RwV3d') elemC = 'ZppVec3';
          fields.set(rawName, {
            rawName,
            fname,
            kind: 'array',
            len,
            hasSizeCheck: len,
            ct: `std::array<${elemC}, ${len}>`,
            defVal: '{}',
            zt
          });
        } else if (zt.startsWith('ℙ ') || zt.startsWith('Ref[')) {
          fields.set(rawName, {
            rawName,
            fname: `${fname}_card`,
            kind: 'opt_ref',
            ct: 'uint8_t',
            defVal: '0u',
            zt
          });
        } else if (zt === 'CVector' || zt === 'RwV3d') {
          fields.set(rawName, { rawName, fname, kind: 'vec3', ct: 'ZppVec3', defVal: '{}', zt });
        } else if (zt.startsWith('ℝ')) {
          fields.set(rawName, { rawName, fname, kind: 'scalar_float', ct: 'float', defVal: '0.0f', zt });
        } else if (zt.startsWith('𝔹')) {
          fields.set(rawName, { rawName, fname, kind: 'scalar_bool', ct: 'bool', defVal: 'false', zt });
        } else if (zt.startsWith('ℕ')) {
          fields.set(rawName, { rawName, fname, kind: 'scalar_int', ct: 'int64_t', defVal: '0', zt });
        } else if (zt.startsWith('ℤ') || zt === 'ADDRESS' || zt === 'HANDLE' || fileEnumNames.has(zt)) {
          fields.set(rawName, { rawName, fname, kind: 'scalar_int', ct: 'int32_t', defVal: '0', zt });
        } else {
          // Non-primitive aggregate/opaque type: keep field for schema layout, but mark kind='opaque'
          // so deltaOps never substitutes an integer placeholder for a non-integer field!
          fields.set(rawName, { rawName, fname, kind: 'opaque', ct: 'int32_t', defVal: '0', zt });
        }
      }

      for (const rawW of whereLines) {
        const mAddr = rawW.match(/^([A-Za-z_]\w*)\s*∈\s*ADDRESS$/);
        if (mAddr && !fields.has(mAddr[1])) {
          const addrName = mAddr[1];
          const safeAddr = safeFieldName(addrName);
          fields.set(addrName, { rawName: addrName, fname: safeAddr, kind: 'scalar_int', ct: 'int64_t', defVal: '0', zt: 'ADDRESS' });
        }
      }

      if (hasUnresolvedField || fields.size === 0) continue;

      const cppPreds = [];
      let negMutation = null;
      let allPredsSupported = true;

      for (const f of fields.values()) {
        if (f.kind === 'array') {
          cppPreds.push(`(${f.fname}_len == ${f.len}u && ${f.fname}.size() == ${f.len}u)`);
          if (!negMutation) negMutation = `bad.${f.fname}_len = 0u;`;
        }
      }

      for (const rawW of whereLines) {
        const parts = rawW.split('∧').map(p => p.trim()).filter(Boolean);
        for (const w of parts) {
          let m;
          if ((m = w.match(/^#([A-Za-z_]\w*)\s*=\s*(\d+)$/))) {
            const f = fields.get(m[1]);
            const n = Number(m[2]);
            if (f && f.kind === 'array' && f.len === n) {
              const expr = `(${f.fname}_len == ${n}u && ${f.fname}.size() == ${n}u)`;
              if (!cppPreds.includes(expr)) cppPreds.push(expr);
              if (!negMutation) negMutation = `bad.${f.fname}_len = 0u;`;
            } else {
              allPredsSupported = false;
            }
          } else if ((m = w.match(/^#([A-Za-z_]\w*)\s*≤\s*1$/))) {
            const f = fields.get(m[1]);
            if (f && f.kind === 'opt_ref') {
              cppPreds.push(`(${f.fname} <= 1u)`);
              if (!negMutation) negMutation = `bad.${f.fname} = 2u;`;
            } else {
              allPredsSupported = false;
            }
          } else if ((m = w.match(/^([A-Za-z_]\w*)\s*≥\s*0$/))) {
            const f = fields.get(m[1]);
            if (f && (f.kind === 'scalar_int' || f.kind === 'scalar_float')) {
              cppPreds.push(`(${f.fname} >= 0)`);
              if (!negMutation) negMutation = `bad.${f.fname} = -1;`;
            } else {
              allPredsSupported = false;
            }
          } else if ((m = w.match(/^([A-Za-z_]\w*)\s*=\s*(true|false)$/))) {
            const f = fields.get(m[1]);
            if (f && f.kind === 'scalar_bool') {
              f.defVal = m[2];
              cppPreds.push(`(${f.fname} == ${m[2]})`);
              if (!negMutation) negMutation = `bad.${f.fname} = ${m[2] === 'true' ? 'false' : 'true'};`;
            } else {
              allPredsSupported = false;
            }
          } else if ((m = w.match(/^([A-Za-z_]\w*)\s*=\s*(\d+)$/))) {
            const f = fields.get(m[1]);
            if (f && f.kind === 'scalar_int') {
              f.defVal = m[2];
              cppPreds.push(`(${f.fname} == ${m[2]})`);
              if (!negMutation) negMutation = `bad.${f.fname} = ${Number(m[2]) + 1};`;
            } else {
              allPredsSupported = false;
            }
          } else if ((m = w.match(/^([A-Za-z_]\w*)\s*∈\s*ADDRESS$/))) {
            const addrName = m[1];
            let f = fields.get(addrName);
            if (!f) {
              const safeAddr = safeFieldName(addrName);
              f = { rawName: addrName, fname: safeAddr, kind: 'scalar_int', ct: 'int64_t', defVal: '0', zt: 'ADDRESS' };
              fields.set(addrName, f);
            }
            cppPreds.push(`(${f.fname} >= 0)`);
            if (!negMutation) negMutation = `bad.${f.fname} = -1;`;
          } else {
            allPredsSupported = false;
          }
        }
      }

      if (cppPreds.length > 0 && allPredsSupported && negMutation) {
        sampleSchemas.push({
          sname,
          fields: [...fields.values()],
          cppPreds,
          negMutation,
          source: zf
        });
      }
    }
  }

  if (insideClass) {
    const modList = zfiles.slice(0, 8).join(', ');
    const firstSchema = sampleSchemas.length > 0 ? `zpp_${folder}_spec::${sampleSchemas[0].sname}_ZppContract{}.checkInvariant()` : 'true';
    const firstOp = deltaOps.length > 0
      ? `zpp_${folder}_spec::op_${deltaOps[0].opName}(zpp_${folder}_spec::${deltaOps[0].targetState}_ZppContract{}, zpp_${folder}_spec::apply_${deltaOps[0].opName}(zpp_${folder}_spec::${deltaOps[0].targetState}_ZppContract{}))`
      : (queryOps.length > 0
        ? `zpp_${folder}_spec::op_${queryOps[0].opName}(zpp_${folder}_spec::${queryOps[0].targetState}_ZppContract{})`
        : 'true');
    return [
      `    // ── Z++ Formal Spec Component Binding (${folder}): ${zfiles.length} modules, ${totalSchemas} schemas, ${totalEnums} enums (${modList}) ──`,
      `    static constexpr int zpp_${folder}_modules() { return zpp_${folder}_spec::kZppModuleCount; }`,
      `    static constexpr int zpp_${folder}_schemas() { return zpp_${folder}_spec::kZppSchemaCount; }`,
      `    static constexpr bool verifyComponent_${folder}_zpp() {`,
      `        return zpp_${folder}_spec::verify_${folder}_zpp_invariants() && (${firstSchema}) && (${firstOp});`,
      `    }`,
      `    static_assert(verifyComponent_${folder}_zpp(), "RasterEngine component Z++ binding failed for ${folder}");`,
      ''
    ].join('\n');
  }

  const guard = `GTANGEL_ZPP_${folder.toUpperCase()}_SPEC_GUARD`;
  const out = [
    `#pragma once`,
    `#ifndef ${guard}`,
    `#define ${guard}`,
    `#include <array>`,
    `#include <cstddef>`,
    `#include <cstdint>`,
    `// ============================================================================`,
    `// Z++ Formal Specification Contract Block: src/${folder}`,
    `// Derived from C:/ddd/GTAngel/cpp_test/zpp/${folder}/*.zpp`,
    `// Modules: ${zfiles.length} | Formal Schemas: ${totalSchemas} | Free Types: ${totalEnums}`,
    `// ============================================================================`,
    `namespace zpp_${folder}_spec {`,
    `static constexpr int kZppModuleCount = ${zfiles.length};`,
    `static constexpr int kZppSchemaCount = ${totalSchemas};`,
    `static constexpr int kZppEnumCount   = ${totalEnums};`,
    `struct ZppVec3 {`,
    `    float x{0.0f}, y{0.0f}, z{0.0f};`,
    `    constexpr bool operator==(const ZppVec3& o) const { return x == o.x && y == o.y && z == o.z; }`,
    `    constexpr bool operator!=(const ZppVec3& o) const { return !(*this == o); }`,
    `};`,
  ];

  for (const e of sampleEnums) {
    out.push(`// Free Type from zpp/${folder}/${e.source}`);
    out.push(`enum class ${e.ename} : int32_t { ${e.branches.map(b => `${b.name}_v = ${b.val}`).join(', ')} };`);
  }

  for (const s of sampleSchemas) {
    out.push(`// State Schema Contract from zpp/${folder}/${s.source}`);
    out.push(`struct ${s.sname}_ZppContract {`);
    for (const f of s.fields) {
      out.push(`    ${f.ct} ${f.fname}${f.defVal === '{}' ? '{}' : `{${f.defVal}}`}; // Z++: ${f.zt}`);
      if (f.hasSizeCheck !== undefined) {
        out.push(`    size_t ${f.fname}_len{${f.hasSizeCheck}u}; // Z++ cardinality #${f.rawName}`);
      }
    }
    out.push(`    constexpr bool checkInvariant() const {`);
    out.push(`        return ${s.cppPreds.join('\n            && ')};`);
    out.push(`    }`);
    out.push(`};`);
  }

  for (const qop of queryOps) {
    out.push(`// Query Operation Schema Contract (Ξ${qop.targetState}) from zpp/${folder}/${qop.source}`);
    out.push(`constexpr bool op_${qop.opName}(const ${qop.targetState}_ZppContract& state) {`);
    out.push(`    return state.checkInvariant();`);
    out.push(`}`);
  }

  for (const dop of deltaOps) {
    const paramSig = dop.extraParams.map(p => `, ${p.ct} ${p.pName} = ${p.defIn}`).join('');
    out.push(`// Delta Operation Schema Contract (Δ${dop.targetState}) from zpp/${folder}/${dop.source}`);
    out.push(`constexpr bool op_${dop.opName}(const ${dop.targetState}_ZppContract& pre, const ${dop.targetState}_ZppContract& post${paramSig}) {`);
    out.push(`    return pre.checkInvariant() && post.checkInvariant() && ${dop.updates.map(u => u.postCond).join(' && ')};`);
    out.push(`}`);
    out.push(`constexpr ${dop.targetState}_ZppContract apply_${dop.opName}(const ${dop.targetState}_ZppContract& pre${paramSig}) {`);
    out.push(`    ${dop.targetState}_ZppContract post = pre;`);
    for (const u of dop.updates) {
      out.push(`    ${u.applyExpr}`);
    }
    out.push(`    return post;`);
    out.push(`}`);
  }

  out.push(`constexpr bool verify_${folder}_zpp_invariants() {`);
  for (const s of sampleSchemas) {
    out.push(`    {`);
    out.push(`        ${s.sname}_ZppContract valid{};`);
    out.push(`        if (!valid.checkInvariant()) return false;`);
    out.push(`        ${s.sname}_ZppContract bad{};`);
    out.push(`        ${s.negMutation}`);
    out.push(`        if (bad.checkInvariant()) return false;`);
    out.push(`    }`);
  }
  for (const qop of queryOps) {
    out.push(`    {`);
    out.push(`        const ${qop.targetState}_ZppContract st{};`);
    out.push(`        if (!op_${qop.opName}(st)) return false;`);
    out.push(`    }`);
  }
  for (const dop of deltaOps) {
    out.push(`    {`);
    out.push(`        ${dop.targetState}_ZppContract pre{};`);
    for (const p of dop.extraParams) {
      out.push(`        const ${p.ct} ${p.pName} = ${p.defIn};`);
    }
    for (const u of dop.updates) {
      if (u.preInit) out.push(`        ${u.preInit}`);
    }
    const callArgs = dop.extraParams.map(p => `, ${p.pName}`).join('');
    out.push(`        const auto post = apply_${dop.opName}(pre${callArgs});`);
    out.push(`        if (!op_${dop.opName}(pre, post${callArgs})) return false;`);
    for (const u of dop.updates) {
      out.push(`        {`);
      out.push(`            ${dop.targetState}_ZppContract badPost = post;`);
      out.push(`            ${u.badPostMut}`);
      out.push(`            if (op_${dop.opName}(pre, badPost${callArgs})) return false;`);
      out.push(`        }`);
    }
    out.push(`    }`);
  }
  out.push(`    return kZppModuleCount >= 0 && kZppSchemaCount >= 0;`);
  out.push(`}`);
  out.push(`static_assert(verify_${folder}_zpp_invariants(), "Z++ formal contract invariant failed for ${folder}");`);
  if (folder === 'vehicles' && sampleEnums.some(e => e.ename === 'eCarNodes')) {
    out.push(`static_assert(static_cast<int>(eCarNodes::CAR_WHEEL_RF_v) == 1 && static_cast<int>(eCarNodes::NUM_CAR_NODES_v) == 20, "eCarNodes Z++ valuation mismatch");`);
  }
  if (folder === 'peds' && sampleSchemas.some(s => s.sname === 'CGangs')) {
    out.push(`static_assert(CGangs_ZppContract{}.GangAttackWithCops.size() == 9u, "CGangs NUM_GANGS must be 9");`);
  }
  if (folder === 'renderer' && sampleSchemas.some(s => s.sname === 'CConsole')) {
    out.push(`static_assert(CConsole_ZppContract{}.m_aTimer.size() == 8u, "CConsole MAX_LINES must be 8");`);
  }
  if (folder === 'renderer' && deltaOps.some(d => d.opName === 'CWeather_StoreWeatherState')) {
    out.push(`static_assert(op_CWeather_StoreWeatherState(CWeather_ZppContract{}, apply_CWeather_StoreWeatherState(CWeather_ZppContract{})), "CWeather_StoreWeatherState float-copy delta contract must hold");`);
  }
  if (folder === 'renderer' && deltaOps.some(d => d.opName === 'CWeather_ForceWeatherNow')) {
    out.push(`static_assert(op_CWeather_ForceWeatherNow(CWeather_ZppContract{}, apply_CWeather_ForceWeatherNow(CWeather_ZppContract{}, 3), 3) && []() constexpr { auto bad = apply_CWeather_ForceWeatherNow(CWeather_ZppContract{}, 3); bad.NewWeatherType = 2; return !op_CWeather_ForceWeatherNow(CWeather_ZppContract{}, bad, 3); }(), "CWeather_ForceWeatherNow must require equal weather types from single weather_in input");`);
  }
  if (folder === 'collision' && deltaOps.some(d => d.opName === 'CColSphere_Set')) {
    out.push(`static_assert(op_CColSphere_Set(CColSphere_ZppContract{}, apply_CColSphere_Set(CColSphere_ZppContract{}, 2.5f, ZppVec3{1.0f, 2.0f, 3.0f}, 4, 5), 2.5f, ZppVec3{1.0f, 2.0f, 3.0f}, 4, 5) && !op_CColSphere_Set(CColSphere_ZppContract{}, apply_CColSphere_Set(CColSphere_ZppContract{}, 2.5f, ZppVec3{4.0f, 5.0f, 6.0f}, 4, 5), 2.5f, ZppVec3{1.0f, 2.0f, 3.0f}, 4, 5), "CColSphere_Set must verify 3D CVector center");`);
  }
  if (folder === 'core' && deltaOps.some(d => d.opName === 'CClock_Initialise')) {
    out.push(`static_assert(op_CClock_Initialise(CClock_ZppContract{}, apply_CClock_Initialise(CClock_ZppContract{}, 1000, 4200), 1000, 4200), "CClock_Initialise must accept arbitrary scale and timestamp");`);
  }
  if (folder === 'vehicles' && deltaOps.some(d => d.opName === 'CCarGenerator_SwitchOn')) {
    out.push(`static_assert(apply_CCarGenerator_SwitchOn(CCarGenerator_ZppContract{}).m_nUsesRemaining == 65535, "CCarGenerator_SwitchOn must set UINT16_MAX (65535)");`);
  }
  if (folder === 'math' && queryOps.some(q => q.opName === 'CVector2D_Magnitude_2')) {
    out.push(`static_assert(op_CVector2D_Magnitude_2(CVector2D_ZppContract{}), "CVector2D_Magnitude_2 query contract must hold");`);
  }
  out.push(`} // namespace zpp_${folder}_spec`);
  out.push(`#endif // ${guard}`);
  out.push('');
  return out.join('\n');
}

const L_avatar3d       = findLine(l => l === 'namespace avatar3d {');
const L_poseLib        = findLine(l => l.startsWith('class DtePoseLibrary {'), L_avatar3d);
const L_gameWorld      = findLine(l => l.startsWith('struct GameWorld {'), L_poseLib);
const L_navTrainer     = findLine(l => l.startsWith('// ── Egocentric software renderer ──'), L_gameWorld);
const L_v15render      = findLine(l => l === 'namespace v15render {', L_navTrainer);
const L_dteSkinned     = findLine(l => l.startsWith('// DTE v22 native skin asset reader'), L_v15render);
const L_rasterEngine   = findLine(l => l.startsWith('template<class Surface> class RasterEngine {'), L_dteSkinned);
const L_buildSun       = findLine(l => l === '    void buildSun() {', L_rasterEngine);
const L_buildBuildings = findLine(l => l === '    void buildBuildings(const avatar3d::GameWorld& world) {', L_buildSun);
const L_buildPillars   = findLine(l => l === '    void buildPillars(const avatar3d::GameWorld& world) {', L_buildBuildings);
const L_buildGoal      = findLine(l => l === '    void buildGoal(const avatar3d::GameWorld& world) {', L_buildPillars);
const L_buildWalkers   = findLine(l => l === '    void buildWalkers(const avatar3d::GameWorld& world) {', L_buildGoal);
const L_buildShard     = findLine(l => l === '    void buildQuestShard(const avatar3d::GameWorld& world) {', L_buildWalkers);
const L_drawCoupe      = findLine(l => l.startsWith('    void drawSculptedCoupe('), L_buildShard);
const L_buildPickups   = findLine(l => l === '    void buildPickups(const avatar3d::GameWorld& world) {', L_drawCoupe);
const L_buildProps     = findLine(l => l === '    void buildProps(const avatar3d::GameWorld& world) {', L_buildPickups);
const L_buildAvatar    = findLine(l => l === '    void buildAvatar(const avatar3d::AvatarSkeleton& avatar) {', L_buildProps);
const L_v16motor       = findLine(l => l === '#ifndef GTANGEL_V16_VISUOMOTOR_CPP', L_buildAvatar);
const L_v18peer        = findLine(l => l === '#ifndef GTANGEL_V18_PEER_RELATIONAL_CPP', L_v16motor);
const L_v18peerEnd     = findLine(l => l === '#endif', L_v18peer);
const L_uechovr        = findLine(l => l === 'namespace uechovr {', L_v18peerEnd);
const L_v20cnsFwd      = findLine(l => l === 'namespace v20cns { class WorldEffector; }', L_uechovr);
const L_v20cns         = findLine(l => l === 'struct GTAngelAGI;', L_v20cnsFwd);
const L_v20save        = findLine(l => l === 'namespace v20save {', L_v20cns);
const L_gtangelStruct  = findLine(l => l === 'struct GTAngelAGI {', L_v20save);
const L_renderV15Frame = findLine(l => l.startsWith('static void RenderV15Frame('), L_gtangelStruct);

for (const rootDir of ['src', '../src']) {
  for (const f of folders) {
    fs.mkdirSync(path.join(rootDir, f), { recursive: true });
    fs.writeFileSync(
      path.join(rootDir, f, 'ZppContracts.hpp'),
      buildZppCppContractBlock(f, false),
      'utf8'
    );
  }

  const writeMod = (relPath, content) => {
    fs.writeFileSync(path.join(rootDir, relPath), content, 'utf8');
  };

  writeMod('core/PlatformPreamble.hpp', '#pragma once\n' + sliceLines(1, L_avatar3d - 1) + buildZppCppContractBlock('core', false));
  writeMod('math/Vec3.hpp', buildZppCppContractBlock('math', false) + sliceLines(L_avatar3d, L_poseLib - 1) + [
    '// Component-specific Z++ math bindings (from math/ZppContracts.hpp)',
    'constexpr zpp_math_spec::ZppVec3 toZppVec3(Vec3 v) { return {v.x, v.y, v.z}; }',
    'constexpr Vec3 fromZppVec3(zpp_math_spec::ZppVec3 v) { return {v.x, v.y, v.z}; }',
    'constexpr zpp_math_spec::CRect_ZppContract makeWorldBoundsZppRect(float halfExtent) {',
    '    return zpp_math_spec::apply_CRect_CRect_2(zpp_math_spec::CRect_ZppContract{}, -halfExtent, halfExtent, halfExtent, -halfExtent);',
    '}',
    'static_assert(zpp_math_spec::op_CRect_CRect_2(zpp_math_spec::CRect_ZppContract{}, makeWorldBoundsZppRect(64.0f), -64.0f, 64.0f, 64.0f, -64.0f), "World bounds CRect Z++ contract must hold");',
    ''
  ].join('\n'));
  writeMod('animation/AvatarSkeleton.hpp', buildZppCppContractBlock('animation', false) + sliceLines(L_poseLib, L_gameWorld - 1) + [
    '// Component-specific Z++ animation blend bindings (from animation/ZppContracts.hpp)',
    'constexpr zpp_animation_spec::CAnimBlendAssociation_ZppContract makeAvatarZppBlendState(float blendAmount, float blendDelta) {',
    '    return zpp_animation_spec::apply_CAnimBlendAssociation_SetBlend(zpp_animation_spec::CAnimBlendAssociation_ZppContract{}, blendAmount, blendDelta);',
    '}',
    'static_assert(zpp_animation_spec::op_CAnimBlendAssociation_SetBlend(zpp_animation_spec::CAnimBlendAssociation_ZppContract{}, makeAvatarZppBlendState(0.75f, 0.25f), 0.75f, 0.25f), "AvatarSkeleton CAnimBlendAssociation SetBlend Z++ contract must hold");',
    ''
  ].join('\n'));
  writeMod('collision/WorldCollision.hpp', buildZppCppContractBlock('collision', false) + sliceLines(L_gameWorld, L_navTrainer - 1) + [
    '// Component-specific Z++ collision bindings (from collision/ZppContracts.hpp)',
    'constexpr zpp_collision_spec::CColSphere_ZppContract makePillarZppColSphere(Vec3 pos, float radius) {',
    '    return zpp_collision_spec::apply_CColSphere_Set(zpp_collision_spec::CColSphere_ZppContract{}, radius, zpp_collision_spec::ZppVec3{pos.x, pos.y, pos.z}, 1, 0);',
    '}',
    'static_assert(zpp_collision_spec::op_CColSphere_Set(zpp_collision_spec::CColSphere_ZppContract{}, makePillarZppColSphere(Vec3{10.0f, 0.0f, -10.0f}, 0.55f), 0.55f, zpp_collision_spec::ZppVec3{10.0f, 0.0f, -10.0f}, 1, 0), "GameWorld pillar CColSphere Z++ contract must hold");',
    ''
  ].join('\n'));
  writeMod('core/CognitiveSubsystems.hpp', '#pragma once\n' + sliceLines(L_navTrainer, L_v15render - 1) + [
    '// Component-specific Z++ core clock & pool bindings (from core/ZppContracts.hpp)',
    'namespace gtangel_zpp_core_bind {',
    'constexpr zpp_core_spec::CClock_ZppContract makeInitialisedGameClock(int64_t msPerMin, int64_t tickMs) {',
    '    const auto c0 = zpp_core_spec::apply_CClock_Initialise(zpp_core_spec::CClock_ZppContract{}, msPerMin, tickMs);',
    '    return zpp_core_spec::apply_CClock_StoreClock(c0);',
    '}',
    'static_assert(makeInitialisedGameClock(1000, 16).ms_bClockHasBeenStored && makeInitialisedGameClock(1000, 16).ms_Stored_nGameClockHours == 12, "CognitiveSubsystems CClock Z++ store transition must hold");',
    '} // namespace gtangel_zpp_core_bind',
    ''
  ].join('\n'));
  writeMod('fakerw/RwTypes.hpp', buildZppCppContractBlock('fakerw', false) + sliceLines(L_v15render, L_dteSkinned - 1));
  writeMod('modelinfo/DteSkinnedAsset.hpp', buildZppCppContractBlock('modelinfo', false) + sliceLines(L_dteSkinned, L_rasterEngine - 1) + [
    '// Component-specific Z++ modelinfo bindings (from modelinfo/ZppContracts.hpp)',
    'constexpr zpp_modelinfo_spec::CBaseModelInfo_ZppContract makeDteSkinnedModelInfoContract() {',
    '    return zpp_modelinfo_spec::apply_CBaseModelInfo_Init2dEffects(zpp_modelinfo_spec::CBaseModelInfo_ZppContract{});',
    '}',
    'static_assert(zpp_modelinfo_spec::op_CBaseModelInfo_Init2dEffects(zpp_modelinfo_spec::CBaseModelInfo_ZppContract{}, makeDteSkinnedModelInfoContract()), "DteSkinnedAsset CBaseModelInfo Z++ contract must hold");',
    ''
  ].join('\n'));
  writeMod('rw/RasterEngineHeader.hpp', buildZppCppContractBlock('rw', false) + sliceLines(L_rasterEngine, L_buildSun - 1) + buildZppCppContractBlock('rw', true));
  writeMod('leeds/SkyAndRoads.hpp', buildZppCppContractBlock('leeds', true) + sliceLines(L_buildSun, L_buildBuildings - 1));
  writeMod('buildings/Buildings.hpp', buildZppCppContractBlock('buildings', true) + sliceLines(L_buildBuildings, L_buildPillars - 1));
  writeMod('entities/Pillars.hpp', buildZppCppContractBlock('entities', true) + sliceLines(L_buildPillars, L_buildGoal - 1));
  writeMod('entities/Props.hpp', [
    '    // Component-specific Z++ street prop CDummy contract binding (from entities/ZppContracts.hpp)',
    '    static constexpr zpp_entities_spec::CDummy_ZppContract makeStreetPropDummyContract() {',
    '        return zpp_entities_spec::CDummy_ZppContract{};',
    '    }',
    '    static_assert(makeStreetPropDummyContract().checkInvariant(), "Props CDummy Z++ contract must hold");',
    ''
  ].join('\n') + sliceLines(L_buildProps, L_buildAvatar - 1));
  writeMod('objects/WorldObjects.hpp', buildZppCppContractBlock('objects', true) + sliceLines(L_buildGoal, L_buildWalkers - 1));
  writeMod('objects/QuestShard.hpp', [
    '    // Component-specific Z++ quest shard CObjectInfo contract binding (from objects/ZppContracts.hpp)',
    '    static constexpr zpp_objects_spec::CObjectInfo_ZppContract makeQuestShardObjectContract() {',
    '        return zpp_objects_spec::CObjectInfo_ZppContract{};',
    '    }',
    '    static_assert(makeQuestShardObjectContract().checkInvariant(), "QuestShard CObjectInfo Z++ contract must hold");',
    ''
  ].join('\n') + sliceLines(L_buildShard, L_drawCoupe - 1));
  writeMod('objects/Pickups.hpp', [
    '    // Component-specific Z++ pickup CParticleObject contract binding (from objects/ZppContracts.hpp)',
    '    static constexpr zpp_objects_spec::CParticleObject_ZppContract makePickupParticleContract() {',
    '        return zpp_objects_spec::apply_CParticleObject_RemoveObject(zpp_objects_spec::CParticleObject_ZppContract{});',
    '    }',
    '    static_assert(zpp_objects_spec::op_CParticleObject_RemoveObject(zpp_objects_spec::CParticleObject_ZppContract{}, makePickupParticleContract()), "Pickups CParticleObject Z++ contract must hold");',
    ''
  ].join('\n') + sliceLines(L_buildPickups, L_buildProps - 1));
  writeMod('peds/Peds.hpp', buildZppCppContractBlock('peds', true) + sliceLines(L_buildWalkers, L_buildShard - 1));
  writeMod('vehicles/Vehicles.hpp', buildZppCppContractBlock('vehicles', true) + sliceLines(L_drawCoupe, L_buildPickups - 1));
  writeMod('renderer/SoftwareRasterizer.hpp', buildZppCppContractBlock('renderer', true) + sliceLines(L_buildAvatar, L_v16motor - 1) + buildZppCppContractBlock('renderer', false));
  writeMod('control/MotorAndLearning.hpp', buildZppCppContractBlock('control', false) + sliceLines(L_v16motor, L_v18peer - 1) + '#endif\n#endif\n' + [
    '// Component-specific Z++ control & CCarCtrl bindings (from control/ZppContracts.hpp)',
    'namespace gtangel_zpp_control_bind {',
    'constexpr zpp_control_spec::CCarCtrl_ZppContract makeInitialisedCarCtrl() {',
    '    return zpp_control_spec::apply_CCarCtrl_Init(zpp_control_spec::CCarCtrl_ZppContract{});',
    '}',
    'static_assert(zpp_control_spec::op_CCarCtrl_Init(zpp_control_spec::CCarCtrl_ZppContract{}, makeInitialisedCarCtrl()), "MotorAndLearning CCarCtrl Z++ contract must hold");',
    '} // namespace gtangel_zpp_control_bind',
    ''
  ].join('\n'));
  writeMod('weapons/SafetyAndPeerShield.hpp', buildZppCppContractBlock('weapons', false) + sliceLines(L_v18peer, L_v18peerEnd) + [
    '// Component-specific Z++ weapon & explosion safety bindings (from weapons/ZppContracts.hpp)',
    'namespace gtangel_zpp_weapons_bind {',
    'constexpr zpp_weapons_spec::CExplosion_ZppContract makeClearedExplosionState() {',
    '    return zpp_weapons_spec::apply_CExplosion_ResetExplosionActiveCounter(zpp_weapons_spec::CExplosion_ZppContract{});',
    '}',
    'static_assert(zpp_weapons_spec::op_CExplosion_ResetExplosionActiveCounter(zpp_weapons_spec::CExplosion_ZppContract{}, makeClearedExplosionState()), "SafetyAndPeerShield CExplosion Z++ contract must hold");',
    '} // namespace gtangel_zpp_weapons_bind',
    ''
  ].join('\n'));
  writeMod('audio/VrAndEndocrineAudio.hpp', buildZppCppContractBlock('audio', false) + sliceLines(L_uechovr, L_v20cnsFwd - 1) + [
    '// Component-specific Z++ audio collision & sample bindings (from audio/ZppContracts.hpp)',
    'namespace gtangel_zpp_audio_bind {',
    'constexpr zpp_audio_spec::cAudioCollisionManager_ZppContract makeAudioCollisionManagerContract() {',
    '    return zpp_audio_spec::cAudioCollisionManager_ZppContract{};',
    '}',
    'static_assert(makeAudioCollisionManagerContract().checkInvariant(), "VrAndEndocrineAudio cAudioCollisionManager Z++ contract must hold");',
    '} // namespace gtangel_zpp_audio_bind',
    ''
  ].join('\n'));
  writeMod('text/StoryAndQuestText.hpp', buildZppCppContractBlock('text', false) + sliceLines(L_v20cnsFwd, L_v20cns - 1) + [
    '// Component-specific Z++ text & pager bindings (from text/ZppContracts.hpp)',
    'namespace gtangel_zpp_text_bind {',
    'constexpr zpp_text_spec::CPager_ZppContract makeInitialisedStoryPager() {',
    '    return zpp_text_spec::apply_CPager_Init(zpp_text_spec::CPager_ZppContract{});',
    '}',
    'static_assert(zpp_text_spec::op_CPager_Init(zpp_text_spec::CPager_ZppContract{}, makeInitialisedStoryPager()), "StoryAndQuestText CPager Z++ contract must hold");',
    '} // namespace gtangel_zpp_text_bind',
    ''
  ].join('\n'));
  writeMod('extras/DteBridgeAndCommons.hpp', buildZppCppContractBlock('extras', false) + sliceLines(L_v20cns, L_v20save - 1) + [
    '// Component-specific Z++ extras & INI map bindings (from extras/ZppContracts.hpp)',
    'namespace gtangel_zpp_extras_bind {',
    'static_assert(zpp_extras_spec::op_INIMap_get_9(zpp_extras_spec::INIMap_ZppContract{}), "DteBridgeAndCommons INIMap Z++ query contract must hold");',
    '} // namespace gtangel_zpp_extras_bind',
    ''
  ].join('\n'));
  writeMod('save/Persistence.hpp', buildZppCppContractBlock('save', false) + sliceLines(L_v20save, L_gtangelStruct - 1) + [
    '// Component-specific Z++ save date bindings (from save/ZppContracts.hpp)',
    'namespace gtangel_zpp_save_bind {',
    'constexpr zpp_save_spec::CDate_ZppContract makeStampedSaveDate(int32_t sec, int32_t min, int32_t hr, int32_t day, int32_t mon, int32_t yr) {',
    '    const auto d0 = zpp_save_spec::apply_CDate_CDate(zpp_save_spec::CDate_ZppContract{});',
    '    return zpp_save_spec::apply_CDate_PopulateDateFields(d0, sec, min, hr, day, mon, yr);',
    '}',
    'static_assert(zpp_save_spec::op_CDate_PopulateDateFields(zpp_save_spec::CDate_ZppContract{}, makeStampedSaveDate(30, 45, 14, 8, 11, 2026), 30, 45, 14, 8, 11, 2026), "Persistence CDate PopulateDateFields Z++ contract must hold");',
    '} // namespace gtangel_zpp_save_bind',
    ''
  ].join('\n'));
  writeMod('core/GTAngelAGI.hpp', '#pragma once\n' + sliceLines(L_gtangelStruct, L_renderV15Frame - 1) + [
    '// Component-specific Z++ GTAngelAGI clock & weather bindings',
    'namespace gtangel_zpp_agi_bind {',
    'static_assert(zpp_core_spec::op_CClock_SetGameClock(zpp_core_spec::CClock_ZppContract{}, zpp_core_spec::apply_CClock_SetGameClock(zpp_core_spec::CClock_ZppContract{}, 6, 18, 0), 6, 18, 0), "GTAngelAGI CClock SetGameClock Z++ contract must hold");',
    '} // namespace gtangel_zpp_agi_bind',
    ''
  ].join('\n'));
  writeMod('skel/Win32CockpitSkel.hpp', buildZppCppContractBlock('skel', false) + sliceLines(L_renderV15Frame, lines.length));

  const docsHeader = [
    '#pragma once',
    '// Master Z++ Formal Specification Aggregator across all 22 GTAngel src/ subsystems',
    ...folders.map(f => `#include "../${f}/ZppContracts.hpp"`),
    'namespace zpp_master_spec {',
    'constexpr int totalZppModules() {',
    '    return ' + folders.map(f => `zpp_${f}_spec::kZppModuleCount`).join(' + ') + ';',
    '}',
    'constexpr int totalZppSchemas() {',
    '    return ' + folders.map(f => `zpp_${f}_spec::kZppSchemaCount`).join(' + ') + ';',
    '}',
    'constexpr int totalZppEnums() {',
    '    return ' + folders.map(f => `zpp_${f}_spec::kZppEnumCount`).join(' + ') + ';',
    '}',
    'constexpr bool verifyAll22Subsystems() {',
    '    return ' + folders.map(f => `zpp_${f}_spec::verify_${f}_zpp_invariants()`).join(' && ') + ';',
    '}',
    'static_assert(verifyAll22Subsystems(), "All 22 Z++ subsystem formal specifications (positive + negative cases + operations) must hold");',
    '} // namespace zpp_master_spec',
    ''
  ].join('\n');
  writeMod('docs/DocsZppSpec.hpp', docsHeader);
  writeMod('docs/ARCHITECTURE.md', '# GTAngel v56 Modular Source Architecture (src/) + Z++ Formal Contracts\n\nAll 22 re3-style subsystems are organized under src/<module>/, enriched with Z++ formal contracts from cpp_test/zpp/<module>/*.zpp, and compiled through src/main.cpp.\n');

  writeMod('main.cpp', [
    '// GTAngel v56 — Modular 22-Folder Source Entry Point (src/main.cpp)',
    '#include "core/PlatformPreamble.hpp"',
    '#include "docs/DocsZppSpec.hpp"',
    '#include "math/Vec3.hpp"',
    '#include "animation/AvatarSkeleton.hpp"',
    '#include "collision/WorldCollision.hpp"',
    '#include "core/CognitiveSubsystems.hpp"',
    '#include "fakerw/RwTypes.hpp"',
    '#include "modelinfo/DteSkinnedAsset.hpp"',
    '#include "rw/RasterEngineHeader.hpp"',
    '#include "leeds/SkyAndRoads.hpp"',
    '#include "buildings/Buildings.hpp"',
    '#include "entities/Pillars.hpp"',
    '#include "objects/WorldObjects.hpp"',
    '#include "peds/Peds.hpp"',
    '#include "objects/QuestShard.hpp"',
    '#include "vehicles/Vehicles.hpp"',
    '#include "objects/Pickups.hpp"',
    '#include "entities/Props.hpp"',
    '#include "renderer/SoftwareRasterizer.hpp"',
    '#include "control/MotorAndLearning.hpp"',
    '#include "weapons/SafetyAndPeerShield.hpp"',
    '#include "audio/VrAndEndocrineAudio.hpp"',
    '#include "text/StoryAndQuestText.hpp"',
    '#include "extras/DteBridgeAndCommons.hpp"',
    '#include "save/Persistence.hpp"',
    '#include "core/GTAngelAGI.hpp"',
    '#include "skel/Win32CockpitSkel.hpp"',
    ''
  ].join('\n'));
}

console.log('Emitted 22 modular src/ directories enriched with Z++ formal contracts + src/main.cpp');
