import { execSync } from 'child_process';
import { groupBy, mapValues, meanBy } from 'es-toolkit';

const OPTIONS = [
  {
    scheduler: 'fcfs',
  },
  {
    scheduler: 'sjf',
  },
  {
    scheduler: 'psjf',
  },
  {
    scheduler: 'priority',
  },
  {
    scheduler: 'ppriority',
  },
  {
    scheduler: 'rr',
    "rr-time-quantum": 10,
  },
  {
    scheduler: 'rr',
    "rr-time-quantum": 20,
  },
  {
    scheduler: 'rr',
    "rr-time-quantum": 30,
  },
  {
    scheduler: 'rr',
    "rr-time-quantum": 40,
  },
  {
    scheduler: 'rr',
    "rr-time-quantum": 50,
  },
]

function getResult(fileName) {
  const result = [];

  for (const { scheduler, ...rest } of OPTIONS) {
    // console.log(`\n${scheduler}:`);

    try {
      const output = execSync(`../../bin/sim --tsv=${fileName} --scheduler=${scheduler} --non-interactive ${Object.entries(rest).map(([key, value]) => `--${key}=${value}`).join(' ')
        }`, {
        encoding: 'utf-8'
      });

      const statLines = output.split('\n').filter(l => l.includes('[평균 '));

      // console.log(statLines.join('\n'));

      result.push({
        scheduler: `${scheduler}${Object.entries(rest).length > 0 ? ` (${Object.entries(rest).map(([key, value]) => `${key}=${value}`).join(', ')})` : ''}`,
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

for (let i = 0; i < 100; i++) {
  const fileName = `../../data/__generated__/processes_${i}.tsv`;

  console.log(`${fileName} 결과 대기 중`);

  results.push(getResult(fileName));
}

const byScheduler = groupBy(results.flat(), result => result.scheduler);

console.dir(
  mapValues(byScheduler, (group) => ({
    turnaround: meanBy(group, g => g.turnaround).toFixed(2),
    waiting: meanBy(group, g => g.waiting).toFixed(2),
  }))
  , { depth: Infinity }
);
