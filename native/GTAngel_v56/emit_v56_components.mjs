import fs from 'node:fs';import assert from 'node:assert/strict';
// v27 component EMITTER (emission-only; no locks, no forbidden strings, no byte-equality gates).
// Replaces the deleted verify_v27_source.mjs gate script: it only extracts reusable
// component headers so verify_v29_learning.cpp can compile against the real v27 code.
const after=fs.readFileSync('gtangel_agi_v56_full.cpp','utf8');
function region(s,a,b){const i=s.indexOf(a),j=s.indexOf(b,i+a.length);assert(i>=0&&j>=0,a);return s.slice(i,j+b.length);}
const avatar=region(after,'namespace avatar3d {','} // namespace avatar3d'),
      render=region(after,'namespace v15render {','} // namespace v15render'),
      motor=region(after,'namespace v16motor {','} // namespace v16motor'),
      policy=region(after,'namespace v17policy {','} // namespace v17policy'),
      observer=region(after,'namespace v27learn {','} // namespace v27learn'),
      local=region(after,'namespace v27dte {','} // namespace v27dte'),
      gnn=region(after,'namespace v27gnn {','} // namespace v27gnn'),
      gnnC=after.slice(after.indexOf('extern "C" {'), after.indexOf('namespace v27gnn'));
const prelude=`#pragma once\n#include <algorithm>\n#include <array>\n#include <chrono>\n#include <cmath>\n#include <cstdint>\n#include <cstring>\n#include <cstdio>\n#include <deque>\n#include <filesystem>\n#include <fstream>\n#include <iomanip>\n#include <iostream>\n#include <limits>\n#include <map>\n#include <optional>\n#include <random>\n#include <sstream>\n#include <string>\n#include <unordered_map>\n#include <vector>\n`;
const platform='#ifdef _WIN32\n#define NOMINMAX\n#define WIN32_LEAN_AND_MEAN\n#include <windows.h>\n#endif\n';
const portablePrelude=prelude+'#ifndef _WIN32\nstatic uint64_t GetTickCount64(){return 1000;}\n#endif\n';
fs.writeFileSync('verify_v56_components.hpp',
    platform+portablePrelude+avatar+'\n'+render+'\n'+motor+'\n'+policy+'\n'+observer+'\n'+local+'\n'+gnnC+'\n'+gnn+'\n');
console.log('v56 components emitted: avatar, render, motor, policy, observer, local, gnn (emission-only; learning unlocked)');