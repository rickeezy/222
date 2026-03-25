const topicData = {
  "Atomic Structure": `Atoms contain protons (+), neutrons (0), and electrons (-). Atomic number = protons. Mass number = protons + neutrons. Isotopes are same element, different neutrons.`,
  "Periodic Trends": `Across a period: atomic radius decreases, ionization energy increases, electronegativity increases. Down a group: radius increases, ionization energy usually decreases.`,
  "Chemical Bonding": `Ionic bonding = transfer of electrons (metal + nonmetal). Covalent bonding = sharing electrons (nonmetals). Polarity depends on electronegativity difference and molecular geometry.`,
  Stoichiometry: `Balance equations first. Convert from grams -> moles, use mole ratio from coefficients, then convert to target units. Dimensional analysis is your best friend.`,
  AcidsAndBases: `pH = -log[H+], pOH = -log[OH-], and pH + pOH = 14 at 25°C. Strong acids/bases fully dissociate; weak ones establish equilibrium.`,
  Thermochemistry: `q = m*c*ΔT. Endothermic processes absorb heat; exothermic processes release heat. Enthalpy (ΔH) tracks heat at constant pressure.`,
};

const elements = [
  { n: 1, s: "H", name: "Hydrogen", m: 1.008 },
  { n: 2, s: "He", name: "Helium", m: 4.0026 },
  { n: 3, s: "Li", name: "Lithium", m: 6.94 },
  { n: 4, s: "Be", name: "Beryllium", m: 9.0122 },
  { n: 5, s: "B", name: "Boron", m: 10.81 },
  { n: 6, s: "C", name: "Carbon", m: 12.011 },
  { n: 7, s: "N", name: "Nitrogen", m: 14.007 },
  { n: 8, s: "O", name: "Oxygen", m: 15.999 },
  { n: 9, s: "F", name: "Fluorine", m: 18.998 },
  { n: 10, s: "Ne", name: "Neon", m: 20.18 },
  { n: 11, s: "Na", name: "Sodium", m: 22.99 },
  { n: 12, s: "Mg", name: "Magnesium", m: 24.305 },
  { n: 13, s: "Al", name: "Aluminum", m: 26.982 },
  { n: 14, s: "Si", name: "Silicon", m: 28.085 },
  { n: 15, s: "P", name: "Phosphorus", m: 30.974 },
  { n: 16, s: "S", name: "Sulfur", m: 32.06 },
  { n: 17, s: "Cl", name: "Chlorine", m: 35.45 },
  { n: 18, s: "Ar", name: "Argon", m: 39.948 },
  { n: 19, s: "K", name: "Potassium", m: 39.098 },
  { n: 20, s: "Ca", name: "Calcium", m: 40.078 },
  { n: 26, s: "Fe", name: "Iron", m: 55.845 },
  { n: 29, s: "Cu", name: "Copper", m: 63.546 },
  { n: 30, s: "Zn", name: "Zinc", m: 65.38 },
  { n: 35, s: "Br", name: "Bromine", m: 79.904 },
  { n: 47, s: "Ag", name: "Silver", m: 107.8682 },
  { n: 53, s: "I", name: "Iodine", m: 126.90447 },
  { n: 79, s: "Au", name: "Gold", m: 196.96657 },
];

const massLookup = Object.fromEntries(elements.map((e) => [e.s, e.m]));

const picker = document.getElementById("topic-picker");
const topicContent = document.getElementById("topic-content");
Object.keys(topicData).forEach((k) => {
  const option = document.createElement("option");
  option.value = k;
  option.textContent = k;
  picker.appendChild(option);
});

function updateTopic() {
  topicContent.textContent = topicData[picker.value] || "Select a topic.";
}
picker.addEventListener("change", updateTopic);
updateTopic();

const elementSearch = document.getElementById("element-search");
const elementResults = document.getElementById("element-results");
function renderElements(q = "") {
  const query = q.trim().toLowerCase();
  const filtered = elements
    .filter((e) =>
      [String(e.n), e.s.toLowerCase(), e.name.toLowerCase()].some((v) =>
        v.includes(query)
      )
    )
    .slice(0, 8);

  elementResults.innerHTML = "";
  filtered.forEach((e) => {
    const div = document.createElement("div");
    div.className = "element-item";
    div.textContent = `${e.n} | ${e.s} | ${e.name} | ${e.m} g/mol`;
    elementResults.appendChild(div);
  });
}

renderElements();
elementSearch.addEventListener("input", (e) => renderElements(e.target.value));

function parseFormula(formula) {
  let i = 0;

  function parseGroup() {
    const counts = {};
    while (i < formula.length) {
      const ch = formula[i];
      if (ch === "(") {
        i++;
        const inner = parseGroup();
        if (formula[i] !== ")") throw new Error("Missing closing parenthesis");
        i++;
        const multiplier = parseNumber();
        Object.entries(inner).forEach(([el, c]) => {
          counts[el] = (counts[el] || 0) + c * multiplier;
        });
      } else if (ch === ")") {
        break;
      } else if (/[A-Z]/.test(ch)) {
        let sym = ch;
        i++;
        if (i < formula.length && /[a-z]/.test(formula[i])) {
          sym += formula[i++];
        }
        if (!massLookup[sym]) throw new Error(`Unknown element: ${sym}`);
        const num = parseNumber();
        counts[sym] = (counts[sym] || 0) + num;
      } else {
        throw new Error(`Invalid character: ${ch}`);
      }
    }
    return counts;
  }

  function parseNumber() {
    let num = "";
    while (i < formula.length && /\d/.test(formula[i])) {
      num += formula[i++];
    }
    return num ? Number(num) : 1;
  }

  const result = parseGroup();
  if (i !== formula.length) throw new Error("Malformed formula");
  return result;
}

function molarMass(formula) {
  const counts = parseFormula(formula);
  return Object.entries(counts).reduce(
    (sum, [el, c]) => sum + massLookup[el] * c,
    0
  );
}

document.getElementById("molar-mass-btn").addEventListener("click", () => {
  const formula = document.getElementById("formula-input").value.trim();
  const out = document.getElementById("molar-mass-output");
  try {
    if (!formula) throw new Error("Enter a chemical formula.");
    const mm = molarMass(formula);
    out.textContent = `${formula}: ${mm.toFixed(4)} g/mol`;
  } catch (err) {
    out.textContent = err.message;
  }
});

const stoichOut = document.getElementById("stoich-output");
document.getElementById("to-grams").addEventListener("click", () => {
  const moles = Number(document.getElementById("st-moles").value);
  const mm = Number(document.getElementById("st-mm").value);
  if (moles > 0 && mm > 0) {
    const grams = moles * mm;
    document.getElementById("st-grams").value = grams.toFixed(5);
    stoichOut.textContent = `${moles} mol × ${mm} g/mol = ${grams.toFixed(5)} g`;
  } else {
    stoichOut.textContent = "Provide positive moles and molar mass.";
  }
});

document.getElementById("to-moles").addEventListener("click", () => {
  const grams = Number(document.getElementById("st-grams").value);
  const mm = Number(document.getElementById("st-mm").value);
  if (grams > 0 && mm > 0) {
    const moles = grams / mm;
    document.getElementById("st-moles").value = moles.toFixed(5);
    stoichOut.textContent = `${grams} g ÷ ${mm} g/mol = ${moles.toFixed(5)} mol`;
  } else {
    stoichOut.textContent = "Provide positive grams and molar mass.";
  }
});

function solveProductRelation(ids, outputId, relationText) {
  const values = ids.map((id) => {
    const raw = document.getElementById(id).value;
    return raw === "" ? null : Number(raw);
  });
  const empty = values.filter((v) => v === null).length;
  const out = document.getElementById(outputId);

  if (empty !== 1) {
    out.textContent = "Leave exactly one field blank.";
    return;
  }

  if (values.some((v) => v !== null && (!Number.isFinite(v) || v <= 0))) {
    out.textContent = "All provided values must be positive numbers.";
    return;
  }

  const idx = values.findIndex((v) => v === null);
  const [a, b, c, d] = values;
  let solved = 0;

  if (idx === 0) solved = (c * d) / b;
  if (idx === 1) solved = (c * d) / a;
  if (idx === 2) solved = (a * b) / d;
  if (idx === 3) solved = (a * b) / c;

  document.getElementById(ids[idx]).value = solved.toFixed(6);
  out.textContent = `${relationText} solved ${ids[idx]} = ${solved.toFixed(6)}`;
}

document.getElementById("dilution-btn").addEventListener("click", () => {
  solveProductRelation(["m1", "v1", "m2", "v2"], "dilution-output", "M1V1 = M2V2");
});

document.getElementById("gas-btn").addEventListener("click", () => {
  solveProductRelation(["gas-p", "gas-v", "gas-n", "gas-t"], "gas-output", "PV = nRT (R assumed 1 for ratio form)");
});

document.getElementById("acid-btn").addEventListener("click", () => {
  const ids = ["ph", "poh", "hplus", "ohminus"];
  const vals = Object.fromEntries(
    ids.map((id) => {
      const raw = document.getElementById(id).value;
      return [id, raw === "" ? null : Number(raw)];
    })
  );

  const provided = Object.values(vals).filter((v) => v !== null);
  const out = document.getElementById("acid-output");
  if (provided.length !== 1) {
    out.textContent = "Enter exactly one value.";
    return;
  }

  let pH;
  if (vals.ph !== null) pH = vals.ph;
  if (vals.poh !== null) pH = 14 - vals.poh;
  if (vals.hplus !== null) pH = -Math.log10(vals.hplus);
  if (vals.ohminus !== null) pH = 14 + Math.log10(vals.ohminus);

  const pOH = 14 - pH;
  const hplus = 10 ** -pH;
  const ohminus = 10 ** -pOH;

  document.getElementById("ph").value = pH.toFixed(5);
  document.getElementById("poh").value = pOH.toFixed(5);
  document.getElementById("hplus").value = hplus.toExponential(3);
  document.getElementById("ohminus").value = ohminus.toExponential(3);

  out.textContent = `pH=${pH.toFixed(4)}, pOH=${pOH.toFixed(4)}, [H+]=${hplus.toExponential(3)}, [OH-]=${ohminus.toExponential(3)}`;
});

document.getElementById("yield-btn").addEventListener("click", () => {
  const actual = Number(document.getElementById("actual-yield").value);
  const theoretical = Number(document.getElementById("theoretical-yield").value);
  const out = document.getElementById("yield-output");

  if (actual >= 0 && theoretical > 0) {
    const pct = (actual / theoretical) * 100;
    out.textContent = `Percent yield = ${pct.toFixed(2)}%`;
  } else {
    out.textContent = "Actual must be >= 0 and theoretical must be > 0.";
  }
});

const quizBank = [
  {
    q: "How many moles are in 36.03 g of water (H2O, 18.015 g/mol)?",
    a: 2.0,
    tolerance: 0.02,
  },
  {
    q: "If pH = 3.00, what is [H+] in mol/L? (scientific notation)",
    a: 1e-3,
    tolerance: 5e-5,
  },
  {
    q: "At constant T, if pressure doubles from 1.0 atm to 2.0 atm, volume changes from 4.0 L to what?",
    a: 2.0,
    tolerance: 0.01,
  },
  {
    q: "What is the atomic number of oxygen?",
    a: 8,
    tolerance: 0,
  },
];

let currentQ = null;
document.getElementById("new-question").addEventListener("click", () => {
  currentQ = quizBank[Math.floor(Math.random() * quizBank.length)];
  document.getElementById("quiz-question").textContent = currentQ.q;
  document.getElementById("quiz-feedback").textContent = "";
  document.getElementById("quiz-feedback").className = "feedback";
  document.getElementById("quiz-answer").value = "";
});

document.getElementById("check-answer").addEventListener("click", () => {
  const feedback = document.getElementById("quiz-feedback");
  if (!currentQ) {
    feedback.className = "feedback bad";
    feedback.textContent = "Generate a question first.";
    return;
  }

  const answer = Number(document.getElementById("quiz-answer").value);
  if (!Number.isFinite(answer)) {
    feedback.className = "feedback bad";
    feedback.textContent = "Enter a numeric answer.";
    return;
  }

  const diff = Math.abs(answer - currentQ.a);
  if (diff <= currentQ.tolerance) {
    feedback.className = "feedback good";
    feedback.textContent = "Correct! Great chemistry work.";
  } else {
    feedback.className = "feedback bad";
    feedback.textContent = `Not quite. Expected approximately ${currentQ.a}.`;
  }
});
