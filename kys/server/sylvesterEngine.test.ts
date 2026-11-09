import { describe, expect, it } from "vitest";
import {
  Complex,
  DualTernaryQuinaryLieAlgebra,
  Nonion,
  cAbs,
  cAdd,
  cExpI,
  cMul,
  createIdentityMatrix,
  createZeroMatrix,
  generateClockTowerRun,
  matrixAdd,
  matrixCommutator,
  matrixMul,
  matrixNorm,
  matrixPower,
  matrixScale,
  matrixSub,
  matrixTensorProduct,
  matrixTrace,
  nonionAssociator,
  nonionCommutator,
  sylvesterClockMatrix,
  sylvesterShiftMatrix,
  verifySylvesterRun,
} from "./sylvesterEngine";

describe("Sylvester Clock & Shift Matrix Model", () => {
  it("should generate S_3 clock matrix with 3rd roots of unity", () => {
    const S3 = sylvesterClockMatrix(3);
    expect(S3.length).toBe(3);
    expect(S3[0][0]).toEqual({ re: 1, im: 0 });
    // S_3^3 should equal Identity(3)
    const S3_cubed = matrixPower(S3, 3);
    const I3 = createIdentityMatrix(3);
    expect(matrixNorm(matrixSub(S3_cubed, I3))).toBeLessThan(1e-5);
  });

  it("should generate T_3 shift matrix with cyclic permutation", () => {
    const T3 = sylvesterShiftMatrix(3);
    expect(T3[1][0]).toEqual({ re: 1, im: 0 });
    expect(T3[2][1]).toEqual({ re: 1, im: 0 });
    expect(T3[0][2]).toEqual({ re: 1, im: 0 });
    // T_3^3 should equal Identity(3)
    const T3_cubed = matrixPower(T3, 3);
    const I3 = createIdentityMatrix(3);
    expect(matrixNorm(matrixSub(T3_cubed, I3))).toBeLessThan(1e-5);
  });

  it("should satisfy Sylvester commutation relation S_N * T_N = w * T_N * S_N", () => {
    for (const N of [3, 5]) {
      const S = sylvesterClockMatrix(N);
      const T = sylvesterShiftMatrix(N);
      const w = cExpI((2 * Math.PI) / N);

      const ST = matrixMul(S, T);
      const TS = matrixMul(T, S);
      const wTS = matrixScale(TS, w);

      expect(matrixNorm(matrixSub(ST, wTS))).toBeLessThan(1e-5);
    }
  });
});

describe("Nonion Hypercomplex Algebra", () => {
  it("should construct basis elements and multiply according to Sylvester law", () => {
    const E01 = Nonion.basis(0, 1); // T_3
    const E10 = Nonion.basis(1, 0); // S_3

    // E10 * E01 = S3 * T3
    const prod = E10.mul(E01);
    expect(prod.coeffs[1][1].re).toBeCloseTo(1, 5);

    // Convert E10 to matrix and verify trace is 0
    const M_E10 = E10.toMatrix();
    const tr = matrixTrace(M_E10);
    expect(cAbs(tr)).toBeLessThan(1e-5);
  });

  it("should satisfy associativity for nonion hypercomplex multiplication [A, B, C] = 0", () => {
    const A = Nonion.basis(1, 2);
    const B = Nonion.basis(2, 1);
    const C = Nonion.basis(0, 2);

    const assoc = nonionAssociator(A, B, C);
    expect(assoc.norm()).toBeLessThan(1e-4);
  });

  it("should compute nonion commutator correctly", () => {
    const S = Nonion.basis(1, 0);
    const T = Nonion.basis(0, 1);

    const comm = nonionCommutator(S, T);
    expect(comm.norm()).toBeGreaterThan(0);
  });
});

describe("KSM Niche-Construction Frame-Shifting Clock-Tower", () => {
  it("should generate 36-tick clock-tower run with preserved norms", () => {
    const records = generateClockTowerRun(36);
    expect(records.length).toBe(36);

    for (const rec of records) {
      expect(rec.coherencePassed).toBe(true);
      expect(rec.nicheNorm).toBeGreaterThan(0);
      expect(Math.abs(rec.frameNorm - rec.nicheNorm)).toBeLessThan(1e-4);
    }
  });
});

describe("Dual Ternary-Quinary Lie Algebra & Bilateral Duality", () => {
  it("should compute 15x15 tensor products and satisfy Jacobi identity", () => {
    const lie = new DualTernaryQuinaryLieAlgebra();
    const A = matrixPower(lie.S3, 1);
    const X = matrixPower(lie.S5, 2);
    const B = matrixPower(lie.T3, 2);
    const Y = matrixPower(lie.T5, 1);
    const C = matrixPower(lie.S3, 2);
    const Z = matrixPower(lie.T5, 3);

    const M1 = matrixTensorProduct(A, X);
    const M2 = matrixTensorProduct(B, Y);
    const M3 = matrixTensorProduct(C, Z);

    expect(M1.length).toBe(15);
    expect(lie.verifyJacobi(M1, M2, M3)).toBe(true);
  });

  it("should satisfy bilateral 3-5 commutative-associative duality mapping", () => {
    const lie = new DualTernaryQuinaryLieAlgebra();
    expect(lie.verifyBilateralDuality()).toBe(true);
  });
});

describe("Sylvester & Dual Lie Algebra Invariant Verification", () => {
  it("should verify all 6 Sylvester invariants pass on the 36-tick run", () => {
    const records = generateClockTowerRun(36);
    const results = verifySylvesterRun(records);

    expect(results.length).toBe(6);
    for (const res of results) {
      expect(res.faults).toBe(0);
      expect(res.ticksPassed).toBe(res.ticksChecked);
    }
  });
});
