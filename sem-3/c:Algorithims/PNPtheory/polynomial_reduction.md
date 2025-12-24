# Polynomial Reduction

## Definition
- **Polynomial-time reduction** is a way to transform one problem into another **in polynomial time**.
- Denoted as `A ≤p B` meaning:
  > Problem A can be reduced to problem B in polynomial time.

## Why Important
- Used to prove **NP-Completeness**.
- If `A ≤p B` and B is solvable in polynomial time → A is also solvable in polynomial time.

## Example
- SAT ≤p 3-SAT
- Hamiltonian Path ≤p Hamiltonian Cycle

## Key Points
- A problem is NP-Complete if **every NP problem can be polynomially reduced to it**.
- Reductions are the **core of NP-Completeness proofs**.

