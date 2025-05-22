import { execSync } from 'child_process';

const SCHEDULERS = [
  "fcfs",
  "sjf",
  "psjf",
  "priority",
  "ppriority",
  "rr",
];

function getResult(fileName) {
  const result = [];

  for (const scheduler of SCHEDULERS) {
    console.log(`\n${scheduler}:`);

    try {
      const output = execSync(`../../bin/sim --tsv=${fileName} --scheduler=${scheduler} --non-interactive`, {
        encoding: 'utf-8'
      });

      const statLines = output.split('\n').filter(l => l.includes('[평균 '));

      console.log(statLines.join('\n'));

      result.push({
        scheduler,
        turnaround: parseFloat(statLines[0].split(']')[1].trim()),
        waiting: parseFloat(statLines[1].split(']')[1].trim())
      });

    } catch (error) {
      console.error(`${scheduler} 실행 실패`, error);
    }
  }

  return result;
}

const results = [];

for (let i = 0; i < 30; i++) {
  const fileName = `../../data/__generated__/processes_${i}.tsv`;

  console.log(`\n\n${fileName}:`);

  results.push(getResult(fileName));
}

console.dir(results, { depth: Infinity });
