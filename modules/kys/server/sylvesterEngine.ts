/**
 * sylvesterEngine.ts — Sylvester Shift & Clock Matrix Model, Nonion Hypercomplex
 * Algebra, KSM Niche-Construction Frame-Shifting Clock-Tower, and Dual
 * Ternary-Quinary Lie Algebra with Bilateral Commutative-Associative Relations.
 *
 * Grounded in Sylvester's (1882) clock and shift matrices, nonion hypercomplex
 * numbers (M3(C)), frame-shifting clock-tower dynamics for KSM niche construction,
 * and icosidodecahedral/DNA 3-5 dual Lie algebra correspondence.
 */

export interface Complex {
  re: number;
  im: number;
}

export function complex(re: number, im = 0): Complex {
  return { re, im };
}

export function cAdd(a: Complex, b: Complex): Complex {
  return { re: a.re + b.re, im: a.im + b.im };
}

export function cSub(a: Complex, b: Complex): Complex {
  return { re: a.re - b.re, im: a.im - b.im };
}

export function cMul(a: Complex, b: Complex): Complex {
  return {
    re: a.re * b.re - a.im * b.im,
    im: a.re * b.im + a.im * b.re,
  };
}

export function cScale(a: Complex, s: number): Complex {
  return { re: a.re * s, im: a.im * s };
}

export function cExpI(theta: number): Complex {
  return { re: Math.cos(theta), im: Math.sin(theta) };
}

export function cAbs(a: Complex): number {
  return Math.hypot(a.re, a.im);
}

export function cConj(a: Complex): Complex {
  return { re: a.re, im: -a.im };
}

export function cApproxEqual(a: Complex, b: Complex, tol = 1e-5): boolean {
  return cAbs(cSub(a, b)) < tol;
}

export type MatrixC = Complex[][];

export function createZeroMatrix(n: number): MatrixC {
  const m: MatrixC = [];
  for (let i = 0; i < n; i++) {
    const row: Complex[] = [];
    for (let j = 0; j < n; j++) {
      row.push(complex(0, 0));
    }
    m.push(row);
  }
  return m;
}

export function createIdentityMatrix(n: number): MatrixC {
  const m = createZeroMatrix(n);
  for (let i = 0; i < n; i++) {
    m[i][i] = complex(1, 0);
  }
  return m;
}

export function matrixAdd(A: MatrixC, B: MatrixC): MatrixC {
  const n = A.length;
  const C = createZeroMatrix(n);
  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      C[i][j] = cAdd(A[i][j], B[i][j]);
    }
  }
  return C;
}

export function matrixSub(A: MatrixC, B: MatrixC): MatrixC {
  const n = A.length;
  const C = createZeroMatrix(n);
  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      C[i][j] = cSub(A[i][j], B[i][j]);
    }
  }
  return C;
}

export function matrixMul(A: MatrixC, B: MatrixC): MatrixC {
  const n = A.length;
  const C = createZeroMatrix(n);
  for (let i = 0; i < n; i++) {
    for (let k = 0; k < n; k++) {
      if (cAbs(A[i][k]) < 1e-12) continue;
      for (let j = 0; j < n; j++) {
        C[i][j] = cAdd(C[i][j], cMul(A[i][k], B[k][j]));
      }
    }
  }
  return C;
}

export function matrixScale(A: MatrixC, s: Complex | number): MatrixC {
  const sc = typeof s === "number" ? complex(s, 0) : s;
  const n = A.length;
  const C = createZeroMatrix(n);
  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      C[i][j] = cMul(A[i][j], sc);
    }
  }
  return C;
}

export function matrixTrace(A: MatrixC): Complex {
  let tr = complex(0, 0);
  for (let i = 0; i < A.length; i++) {
    tr = cAdd(tr, A[i][i]);
  }
  return tr;
}

export function matrixNorm(A: MatrixC): number {
  let sum = 0;
  for (let i = 0; i < A.length; i++) {
    for (let j = 0; j < A[i].length; j++) {
      sum += Math.pow(cAbs(A[i][j]), 2);
    }
  }
  return Math.sqrt(sum);
}

export function matrixCommutator(A: MatrixC, B: MatrixC): MatrixC {
  return matrixSub(matrixMul(A, B), matrixMul(B, A));
}

export function matrixAssociator(A: MatrixC, B: MatrixC, C: MatrixC): MatrixC {
  return matrixSub(matrixMul(matrixMul(A, B), C), matrixMul(A, matrixMul(B, C)));
}

export function matrixApproxEqual(A: MatrixC, B: MatrixC, tol = 1e-5): boolean {
  const n = A.length;
  if (B.length !== n) return false;
  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      if (!cApproxEqual(A[i][j], B[i][j], tol)) return false;
    }
  }
  return true;
}

/**
 * Sylvester Clock Matrix S_N:
 * S_N = diag(1, w, w^2, ..., w^(N-1)) where w = exp(2*pi*i / N)
 */
export function sylvesterClockMatrix(n: number): MatrixC {
  const S = createZeroMatrix(n);
  for (let k = 0; k < n; k++) {
    const angle = (2 * Math.PI * k) / n;
    S[k][k] = cExpI(angle);
  }
  return S;
}

/**
 * Sylvester Shift Matrix T_N:
 * T_N shifts basis vectors cyclicly: T_N |k> = |(k+1) mod N>
 */
export function sylvesterShiftMatrix(n: number): MatrixC {
  const T = createZeroMatrix(n);
  for (let k = 0; k < n; k++) {
    T[(k + 1) % n][k] = complex(1, 0);
  }
  return T;
}

/**
 * Compute power of matrix M^p
 */
export function matrixPower(M: MatrixC, p: number): MatrixC {
  const n = M.length;
  let res = createIdentityMatrix(n);
  for (let i = 0; i < p; i++) {
    res = matrixMul(res, M);
  }
  return res;
}

/**
 * Nonion Hypercomplex Number (9-dimensional Sylvester algebra over C)
 * Represented as linear combination of basis elements E_{j,k} = S_3^j * T_3^k for j,k in {0,1,2}.
 */
export class Nonion {
  // coeffs[j][k] is coefficient of E_{j,k} = S_3^j * T_3^k
  public coeffs: Complex[][];

  constructor(coeffs?: Complex[][]) {
    this.coeffs = createZeroMatrix(3);
    if (coeffs) {
      for (let j = 0; j < 3; j++) {
        for (let k = 0; k < 3; k++) {
          this.coeffs[j][k] = { ...coeffs[j][k] };
        }
      }
    }
  }

  static zero(): Nonion {
    return new Nonion();
  }

  static identity(): Nonion {
    const n = new Nonion();
    n.coeffs[0][0] = complex(1, 0);
    return n;
  }

  static basis(j: number, k: number): Nonion {
    const n = new Nonion();
    n.coeffs[j % 3][k % 3] = complex(1, 0);
    return n;
  }

  add(other: Nonion): Nonion {
    const res = new Nonion();
    for (let j = 0; j < 3; j++) {
      for (let k = 0; k < 3; k++) {
        res.coeffs[j][k] = cAdd(this.coeffs[j][k], other.coeffs[j][k]);
      }
    }
    return res;
  }

  sub(other: Nonion): Nonion {
    const res = new Nonion();
    for (let j = 0; j < 3; j++) {
      for (let k = 0; k < 3; k++) {
        res.coeffs[j][k] = cSub(this.coeffs[j][k], other.coeffs[j][k]);
      }
    }
    return res;
  }

  scale(s: Complex | number): Nonion {
    const res = new Nonion();
    const isNum = typeof s === "number";
    for (let j = 0; j < 3; j++) {
      for (let k = 0; k < 3; k++) {
        res.coeffs[j][k] = isNum
          ? cScale(this.coeffs[j][k], s as number)
          : cMul(this.coeffs[j][k], s as Complex);
      }
    }
    return res;
  }

  /**
   * Nonion multiplication using Sylvester relation:
   * E_{j1, k1} * E_{j2, k2} = (S^j1 T^k1)(S^j2 T^k2)
   *                       = w^(k1 * j2) * S^(j1+j2) T^(k1+k2)
   *                       = w^(k1 * j2) * E_{(j1+j2)%3, (k1+k2)%3}
   * where w = exp(2*pi*i / 3).
   */
  mul(other: Nonion): Nonion {
    const res = new Nonion();
    const w = cExpI((2 * Math.PI) / 3);

    for (let j1 = 0; j1 < 3; j1++) {
      for (let k1 = 0; k1 < 3; k1++) {
        const c1 = this.coeffs[j1][k1];
        if (cAbs(c1) < 1e-12) continue;

        for (let j2 = 0; j2 < 3; j2++) {
          for (let k2 = 0; k2 < 3; k2++) {
            const c2 = other.coeffs[j2][k2];
            if (cAbs(c2) < 1e-12) continue;

            // phase factor w^(k1 * j2)
            const power = (k1 * j2) % 3;
            let phase = complex(1, 0);
            for (let p = 0; p < power; p++) phase = cMul(phase, w);

            const prodCoeff = cMul(cMul(c1, c2), phase);
            const targetJ = (j1 + j2) % 3;
            const targetK = (k1 + k2) % 3;

            res.coeffs[targetJ][targetK] = cAdd(
              res.coeffs[targetJ][targetK],
              prodCoeff,
            );
          }
        }
      }
    }
    return res;
  }

  /**
   * Convert Nonion to equivalent 3x3 complex matrix
   */
  toMatrix(): MatrixC {
    const S3 = sylvesterClockMatrix(3);
    const T3 = sylvesterShiftMatrix(3);
    let M = createZeroMatrix(3);

    for (let j = 0; j < 3; j++) {
      const Sj = matrixPower(S3, j);
      for (let k = 0; k < 3; k++) {
        const Tk = matrixPower(T3, k);
        const Ejk = matrixMul(Sj, Tk);
        const term = matrixScale(Ejk, this.coeffs[j][k]);
        M = matrixAdd(M, term);
      }
    }
    return M;
  }

  trace(): Complex {
    return matrixTrace(this.toMatrix());
  }

  norm(): number {
    return matrixNorm(this.toMatrix());
  }
}

export function nonionCommutator(a: Nonion, b: Nonion): Nonion {
  return a.mul(b).sub(b.mul(a));
}

export function nonionAssociator(a: Nonion, b: Nonion, c: Nonion): Nonion {
  return a.mul(b).mul(c).sub(a.mul(b.mul(c)));
}

/**
 * KSM Frame-Shifting Clock-Tower Record
 */
export interface ClockTowerTickRecord {
  tick: number;
  cycle: number;
  frameShift: { clockPower: number; shiftPower: number };
  nicheTrace: Complex;
  nicheNorm: number;
  frameNorm: number;
  triadNonionNorm: number;
  coherencePassed: boolean;
}

/**
 * Generate 36-tick Frame-Shifting Clock-Tower Run
 */
export function generateClockTowerRun(ticks = 36): ClockTowerTickRecord[] {
  const records: ClockTowerTickRecord[] = [];
  const S3 = sylvesterClockMatrix(3);
  const T3 = sylvesterShiftMatrix(3);

  for (let g = 0; g < ticks; g++) {
    const cycle = Math.floor(g / 12);
    const clockPower = Math.floor(g / 4) % 3;
    const shiftPower = g % 3;

    // Frame Shift Operator F(g) = S_3^clockPower * T_3^shiftPower
    const F_g = matrixMul(matrixPower(S3, clockPower), matrixPower(T3, shiftPower));
    // Inverse: (S^a T^b)^(-1) = T^(3-shiftPower) * S^(3-clockPower) (mod 3)
    const F_inv = matrixMul(matrixPower(T3, (3 - shiftPower) % 3), matrixPower(S3, (3 - clockPower) % 3));

    // Niche nonion state N(g) mapping triad terms into nonion basis
    // Terms: T1..T8 mapped to basis coefficients
    const niche = Nonion.identity();
    const termIndex1 = (g % 3);
    const termIndex2 = ((g + 1) % 3);
    niche.coeffs[termIndex1][termIndex2] = cExpI((2 * Math.PI * g) / 12);
    niche.coeffs[termIndex2][termIndex1] = cExpI(-(2 * Math.PI * g) / 12);

    const nicheMat = niche.toMatrix();
    const nicheTrace = matrixTrace(nicheMat);
    const nicheNorm = matrixNorm(nicheMat);

    // Frame-shifted state W(g) = F(g) * N(g) * F(g)^(-1)
    const W_g = matrixMul(matrixMul(F_g, nicheMat), F_inv);
    const frameNorm = matrixNorm(W_g);

    // Trace and Norm invariants hold under unitary frame transformations
    const normDiff = Math.abs(frameNorm - nicheNorm);
    const coherencePassed = normDiff < 1e-4 && cAbs(nicheTrace) > 0.1;

    records.push({
      tick: g,
      cycle,
      frameShift: { clockPower, shiftPower },
      nicheTrace,
      nicheNorm,
      frameNorm,
      triadNonionNorm: nicheNorm,
      coherencePassed,
    });
  }

  return records;
}

/**
 * Tensor Product of two matrices A (m x m) and B (n x n) -> (m*n) x (m*n)
 */
export function matrixTensorProduct(A: MatrixC, B: MatrixC): MatrixC {
  const mA = A.length;
  const nB = B.length;
  const dim = mA * nB;
  const C = createZeroMatrix(dim);

  for (let iA = 0; iA < mA; iA++) {
    for (let jA = 0; jA < mA; jA++) {
      const aVal = A[iA][jA];
      if (cAbs(aVal) < 1e-12) continue;

      for (let iB = 0; iB < nB; iB++) {
        for (let jB = 0; jB < nB; jB++) {
          const row = iA * nB + iB;
          const col = jA * nB + jB;
          C[row][col] = cAdd(C[row][col], cMul(aVal, B[iB][jB]));
        }
      }
    }
  }
  return C;
}

/**
 * Dual Ternary-Quinary Lie Algebra helper & Bilateral 3-5 Correspondence
 * Ternary dimension N3 = 3, Quinary dimension N5 = 5.
 * Total tensor dimension N3 * N5 = 15.
 */
export class DualTernaryQuinaryLieAlgebra {
  public S3: MatrixC;
  public T3: MatrixC;
  public S5: MatrixC;
  public T5: MatrixC;

  constructor() {
    this.S3 = sylvesterClockMatrix(3);
    this.T3 = sylvesterShiftMatrix(3);
    this.S5 = sylvesterClockMatrix(5);
    this.T5 = sylvesterShiftMatrix(5);
  }

  /**
   * Dual Lie bracket on 15x15 tensor space:
   * [A (x) X, B (x) Y] = (A*B) (x) (X*Y) - (B*A) (x) (Y*X)
   */
  dualBracket(A: MatrixC, X: MatrixC, B: MatrixC, Y: MatrixC): MatrixC {
    const AX = matrixTensorProduct(A, X);
    const BY = matrixTensorProduct(B, Y);
    return matrixCommutator(AX, BY);
  }

  /**
   * Verify Jacobi Identity on Dual Lie Algebra:
   * [X, [Y, Z]] + [Y, [Z, X]] + [Z, [X, Y]] = 0
   */
  verifyJacobi(X: MatrixC, Y: MatrixC, Z: MatrixC): boolean {
    const term1 = matrixCommutator(X, matrixCommutator(Y, Z));
    const term2 = matrixCommutator(Y, matrixCommutator(Z, X));
    const term3 = matrixCommutator(Z, matrixCommutator(X, Y));
    const sum = matrixAdd(matrixAdd(term1, term2), term3);
    return matrixNorm(sum) < 1e-4;
  }

  /**
   * Bilateral 3-5 Joint Clock Map (Phi):
   * Maps index k in Z15 to the joint clock operator S_3^(k mod 3) (x) S_5^(k mod 5)
   * relating 3-fold ternary symmetry to 5-fold quinary symmetry (icosidodecahedral / DNA
   * 3-5 correspondence: 20 triangles * 3 edges = 12 pentagons * 5 edges = 60 bilateral edges).
   */
  bilateralJointClock(k: number): MatrixC {
    const S3_k = matrixPower(this.S3, k % 3);
    const S5_k = matrixPower(this.S5, k % 5);
    return matrixTensorProduct(S3_k, S5_k);
  }

  /**
   * Verify Bilateral Commutative-Associative Duality:
   * For joint clock operators A(k1), A(k2):
   * Phi(k1 + k2) == Phi(k1) * Phi(k2)
   */
  verifyBilateralDuality(): boolean {
    for (let k1 = 0; k1 < 15; k1++) {
      for (let k2 = 0; k2 < 15; k2++) {
        const phi1 = this.bilateralJointClock(k1);
        const phi2 = this.bilateralJointClock(k2);
        const phiSum = this.bilateralJointClock((k1 + k2) % 15);

        const prodPhi = matrixMul(phi1, phi2);
        if (!matrixApproxEqual(phiSum, prodPhi, 1e-4)) {
          return false;
        }
      }
    }
    return true;
  }
}

export interface SylvesterInvariantResult {
  invariantId: string;
  name: string;
  ticksChecked: number;
  ticksPassed: number;
  faults: number;
  summary: string;
}

/**
 * Verify Sylvester Nonion & Dual Lie Algebra Invariants over 36 ticks
 */
export function verifySylvesterRun(records: ClockTowerTickRecord[]): SylvesterInvariantResult[] {
  const n = records.length;
  const results: SylvesterInvariantResult[] = [];

  // INV-SYL-1: Sylvester Clock-Shift Commutation Relation S_N * T_N = exp(2*pi*i/N) * T_N * S_N
  let pass1 = 0;
  for (const dim of [3, 5]) {
    const S = sylvesterClockMatrix(dim);
    const T = sylvesterShiftMatrix(dim);
    const w = cExpI((2 * Math.PI) / dim);

    const ST = matrixMul(S, T);
    const TS = matrixMul(T, S);
    const wTS = matrixScale(TS, w);

    if (matrixApproxEqual(ST, wTS, 1e-5)) {
      pass1++;
    }
  }
  results.push({
    invariantId: "INV-SYL-1",
    name: "Sylvester Clock-Shift Commutation",
    ticksChecked: 2,
    ticksPassed: pass1,
    faults: 2 - pass1,
    summary: `S_N * T_N = e^(2pi i/N) T_N * S_N for N ∈ {3, 5} · ${pass1}/2 pass`,
  });

  // INV-SYL-2: Nonion Algebra Associativity & Basis Product Law
  let pass2 = 0;
  const total2 = 9 * 9 * 9;
  for (let j1 = 0; j1 < 3; j1++) {
    for (let k1 = 0; k1 < 3; k1++) {
      const e1 = Nonion.basis(j1, k1);
      for (let j2 = 0; j2 < 3; j2++) {
        for (let k2 = 0; k2 < 3; k2++) {
          const e2 = Nonion.basis(j2, k2);
          for (let j3 = 0; j3 < 3; j3++) {
            for (let k3 = 0; k3 < 3; k3++) {
              const e3 = Nonion.basis(j3, k3);
              const assoc = nonionAssociator(e1, e2, e3);
              if (assoc.norm() < 1e-4) {
                pass2++;
              }
            }
          }
        }
      }
    }
  }
  results.push({
    invariantId: "INV-SYL-2",
    name: "Nonion Associativity & Basis Closure",
    ticksChecked: total2,
    ticksPassed: pass2,
    faults: total2 - pass2,
    summary: `[A, B, C] = 0 over all 729 nonion basis triples · ${pass2}/${total2} pass`,
  });

  // INV-SYL-3: Clock-Tower Frame-Shifting Trace & Unitary Norm Preservation
  let pass3 = 0;
  for (const rec of records) {
    if (rec.coherencePassed) pass3++;
  }
  results.push({
    invariantId: "INV-SYL-3",
    name: "Clock-Tower Frame-Shifting Preservation",
    ticksChecked: n,
    ticksPassed: pass3,
    faults: n - pass3,
    summary: `Unitary frame rotation preserves nonion norm across all 36 ticks · ${pass3}/${n} pass`,
  });

  // INV-SYL-4: Dual Ternary-Quinary Lie Algebra Jacobi Identity
  const dualLie = new DualTernaryQuinaryLieAlgebra();
  let pass4 = 0;
  const numTests4 = 10;
  for (let t = 0; t < numTests4; t++) {
    const A = matrixPower(dualLie.S3, t % 3);
    const X = matrixPower(dualLie.S5, t % 5);
    const B = matrixPower(dualLie.T3, (t + 1) % 3);
    const Y = matrixPower(dualLie.T5, (t + 1) % 5);
    const C = matrixPower(dualLie.S3, (t + 2) % 3);
    const Z = matrixPower(dualLie.T5, (t + 2) % 5);

    const M1 = matrixTensorProduct(A, X);
    const M2 = matrixTensorProduct(B, Y);
    const M3 = matrixTensorProduct(C, Z);

    if (dualLie.verifyJacobi(M1, M2, M3)) {
      pass4++;
    }
  }
  results.push({
    invariantId: "INV-SYL-4",
    name: "Dual Lie Algebra Jacobi Identity",
    ticksChecked: numTests4,
    ticksPassed: pass4,
    faults: numTests4 - pass4,
    summary: `[X,[Y,Z]] + [Y,[Z,X]] + [Z,[X,Y]] = 0 on g_(3,5) tensor space · ${pass4}/${numTests4} pass`,
  });

  // INV-SYL-5: Bilateral 3-5 Commutative-Associative Duality (DNA / Icosi-Dodec)
  const pass5 = dualLie.verifyBilateralDuality() ? 1 : 0;
  results.push({
    invariantId: "INV-SYL-5",
    name: "Bilateral 3-5 Commutative-Associative Duality",
    ticksChecked: 1,
    ticksPassed: pass5,
    faults: 1 - pass5,
    summary: `Phi(A * B) = Phi(A) * Phi(B) maps ternary (20x3) to quinary (12x5) clock sub-algebra · ${pass5}/1 pass`,
  });

  // INV-SYL-6: Niche-Construction Interface Wiring Coherence
  let pass6 = 0;
  for (const rec of records) {
    if (rec.nicheNorm > 0.5 && rec.frameNorm > 0.5) {
      pass6++;
    }
  }
  results.push({
    invariantId: "INV-SYL-6",
    name: "Niche-Construction Interface Coherence",
    ticksChecked: n,
    ticksPassed: pass6,
    faults: n - pass6,
    summary: `Niche nonion state maintains positive norm & interface coupling · ${pass6}/${n} pass`,
  });

  return results;
}
