const topics = [
  {
    name: 'Mole Concept',
    body: '1 mole = 6.022×10²³ particles. Convert grams ↔ moles with molar mass: moles = grams / g·mol⁻¹. This powers nearly every stoichiometry problem.'
  },
  {
    name: 'Stoichiometry',
    body: 'Balance the equation first. Convert given quantity to moles, apply mole ratio coefficients, then convert to desired unit (grams, liters, particles).'
  },
  {
    name: 'Acids & Bases',
    body: 'pH = −log₁₀[H⁺], pOH = −log₁₀[OH⁻], and pH + pOH = 14 (at 25°C). Strong acids/bases fully dissociate in introductory problems.'
  },
  {
    name: 'Gas Laws',
    body: 'Use PV=nRT with consistent units: P in atm, V in L, T in K. R = 0.082057 L·atm·mol⁻¹·K⁻¹.'
  },
  {
    name: 'Solutions',
    body: 'Molarity M = moles solute / liters solution. For dilution: M₁V₁ = M₂V₂ (same solute, same temperature assumptions).'
  },
  {
    name: 'Thermochemistry',
    body: 'q = mcΔT for heat transfer, and ΔH describes heat at constant pressure. Sign convention: exothermic negative, endothermic positive.'
  }
];

const elements = [
  ['H', 1, 'Hydrogen', 1.008], ['C', 6, 'Carbon', 12.011], ['N', 7, 'Nitrogen', 14.007],
  ['O', 8, 'Oxygen', 15.999], ['Na', 11, 'Sodium', 22.99], ['Mg', 12, 'Magnesium', 24.305],
  ['Al', 13, 'Aluminum', 26.982], ['Cl', 17, 'Chlorine', 35.45], ['K', 19, 'Potassium', 39.098],
  ['Ca', 20, 'Calcium', 40.078], ['Fe', 26, 'Iron', 55.845], ['Cu', 29, 'Copper', 63.546]
];

const calculators = [
  {
    key: 'molarity',
    label: 'Molarity',
    formula: 'M = n / V',
    fields: [
      { key: 'moles', label: 'Moles of solute (mol)' },
      { key: 'volume', label: 'Volume of solution (L)' }
    ],
    solve: ({ moles, volume }) => {
      const M = moles / volume;
      return `Molarity = ${M.toFixed(4)} M`;
    }
  },
  {
    key: 'dilution',
    label: 'Dilution',
    formula: 'M₁V₁ = M₂V₂ (solve for V₂)',
    fields: [
      { key: 'M1', label: 'Initial concentration M₁ (M)' },
      { key: 'V1', label: 'Initial volume V₁ (L)' },
      { key: 'M2', label: 'Final concentration M₂ (M)' }
    ],
    solve: ({ M1, V1, M2 }) => {
      const V2 = (M1 * V1) / M2;
      return `Final volume V₂ = ${V2.toFixed(4)} L`;
    }
  },
  {
    key: 'idealGas',
    label: 'Ideal Gas',
    formula: 'PV = nRT (solve for n)',
    fields: [
      { key: 'P', label: 'Pressure P (atm)' },
      { key: 'V', label: 'Volume V (L)' },
      { key: 'T', label: 'Temperature T (K)' }
    ],
    solve: ({ P, V, T }) => {
      const R = 0.082057;
      const n = (P * V) / (R * T);
      return `Moles of gas n = ${n.toFixed(4)} mol`;
    }
  },
  {
    key: 'ph',
    label: 'pH',
    formula: 'pH = −log₁₀[H⁺]',
    fields: [
      { key: 'H', label: '[H⁺] concentration (M)' }
    ],
    solve: ({ H }) => {
      const pH = -Math.log10(H);
      return `pH = ${pH.toFixed(4)}`;
    }
  },
  {
    key: 'stoichMass',
    label: 'Stoich Mass',
    formula: 'target grams = given grams ÷ MM(given) × ratio × MM(target)',
    fields: [
      { key: 'givenMass', label: 'Given mass (g)' },
      { key: 'givenMM', label: 'Given molar mass (g/mol)' },
      { key: 'ratio', label: 'Mole ratio (target/given)' },
      { key: 'targetMM', label: 'Target molar mass (g/mol)' }
    ],
    solve: ({ givenMass, givenMM, ratio, targetMM }) => {
      const result = (givenMass / givenMM) * ratio * targetMM;
      return `Target mass = ${result.toFixed(4)} g`;
    }
  },
  {
    key: 'heat',
    label: 'Heat q',
    formula: 'q = mcΔT',
    fields: [
      { key: 'm', label: 'Mass m (g)' },
      { key: 'c', label: 'Specific heat c (J/g·°C)' },
      { key: 'dT', label: 'Temperature change ΔT (°C)' }
    ],
    solve: ({ m, c, dT }) => {
      const q = m * c * dT;
      return `Heat transfer q = ${q.toFixed(4)} J`;
    }
  }
];

let activeTopic = 0;
let activeCalc = calculators[0].key;

const topicGrid = document.getElementById('topic-grid');
const topicContent = document.getElementById('topic-content');
const periodicGrid = document.getElementById('periodic-grid');
const calcTabs = document.getElementById('calculator-tabs');
const calcPanel = document.getElementById('calculator-panel');

function renderTopics() {
  topicGrid.innerHTML = '';
  topics.forEach((topic, i) => {
    const button = document.createElement('button');
    button.className = `topic-btn ${i === activeTopic ? 'active' : ''}`;
    button.textContent = topic.name;
    button.addEventListener('click', () => {
      activeTopic = i;
      renderTopics();
    });
    topicGrid.appendChild(button);
  });

  topicContent.innerHTML = `
    <h3>${topics[activeTopic].name}</h3>
    <p>${topics[activeTopic].body}</p>
  `;
}

function renderPeriodic() {
  periodicGrid.innerHTML = '';
  elements.forEach(([symbol, number, name, mass]) => {
    const tile = document.createElement('div');
    tile.className = 'element';
    tile.innerHTML = `
      <small>Atomic #${number}</small>
      <h3>${symbol}</h3>
      <div>${name}</div>
      <small>Atomic Mass: ${mass}</small>
    `;
    periodicGrid.appendChild(tile);
  });
}

function renderCalculatorTabs() {
  calcTabs.innerHTML = '';
  calculators.forEach((calc) => {
    const tab = document.createElement('button');
    tab.className = `calc-tab ${calc.key === activeCalc ? 'active' : ''}`;
    tab.textContent = calc.label;
    tab.addEventListener('click', () => {
      activeCalc = calc.key;
      renderCalculatorTabs();
      renderCalculatorPanel();
    });
    calcTabs.appendChild(tab);
  });
}

function parseInputs(form, fields) {
  const values = {};
  for (const field of fields) {
    const raw = form.elements[field.key].value;
    const value = Number(raw);
    if (!Number.isFinite(value) || raw === '') {
      throw new Error(`Enter a valid number for "${field.label}".`);
    }
    if (value <= 0 && field.key !== 'dT') {
      throw new Error(`"${field.label}" must be greater than zero.`);
    }
    values[field.key] = value;
  }
  return values;
}

function renderCalculatorPanel() {
  const calc = calculators.find(c => c.key === activeCalc);
  const fieldsMarkup = calc.fields.map(field => `
    <label>
      ${field.label}
      <input name="${field.key}" type="number" step="any" required />
    </label>
  `).join('');

  calcPanel.innerHTML = `
    <h3>${calc.label} Calculator</h3>
    <p><strong>Formula:</strong> ${calc.formula}</p>
    <form id="calc-form">
      <div class="form-grid">${fieldsMarkup}</div>
      <button class="solve" type="submit">Solve</button>
    </form>
    <div id="calc-result" class="result">Enter values and click <strong>Solve</strong>.</div>
  `;

  const form = document.getElementById('calc-form');
  const result = document.getElementById('calc-result');

  form.addEventListener('submit', (event) => {
    event.preventDefault();
    try {
      const values = parseInputs(form, calc.fields);
      result.classList.remove('error');
      result.innerHTML = `<strong>${calc.solve(values)}</strong>`;
    } catch (error) {
      result.classList.add('error');
      result.textContent = error.message;
    }
  });
}

renderTopics();
renderPeriodic();
renderCalculatorTabs();
renderCalculatorPanel();
