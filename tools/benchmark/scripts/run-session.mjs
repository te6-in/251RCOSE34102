import { execSync } from 'child_process';

const schedulers = [
  "fcfs",
  "sjf",
  "psjf",
  "priority",
  "ppriority",
  "rr",
];

const file = "../../data/__generated__/processes_0.tsv";

for (const scheduler of schedulers) {
  console.log(`\n${scheduler}:`);

  try {
    const output = execSync(`../../bin/sim --tsv=${file} --scheduler=${scheduler} --non-interactive`, {
      encoding: 'utf-8'
    });

    const statLines = output.split('\n').filter(l => l.includes('[평균'));
    console.log(statLines.join('\n'));
  } catch (e) {
    console.error(`${scheduler} 실행 실패:\n${e.stdout || e.message}`);
  }
}
