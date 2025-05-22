import fs from 'fs';
import path from 'path';

function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

function generateProcesses(processCount) {
  let lines = [
    "# pid  arrived_at   priority   cpu_burst  io_burst   io_request_time"
  ];

  for (let i = 0; i < processCount; i++) {
    const pid = 10000 + i;
    const arrived_at = getRandomInt(0, 500);
    const priority = getRandomInt(1, 10);
    const cpu_burst = getRandomInt(1, 500);
    const io_burst = getRandomInt(0, 1) === 0 ? 0 : getRandomInt(1, 100);
    const io_request_time = io_burst > 0 ? getRandomInt(0, cpu_burst) : 0;

    lines.push(`${pid} ${arrived_at} ${priority} ${cpu_burst} ${io_burst} ${io_request_time}`);
  }

  return lines.join('\n') + '\n';
}

function generateTsv({ fileCount, processCount }) {
  const dir = '../../data/__generated__/';

  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir);
  }

  for (let i = 0; i < fileCount; i++) {
    const fileName = path.join(dir, `processes_${i}.tsv`);
    const data = generateProcesses(processCount);

    fs.writeFileSync(fileName, data);
    console.log(`${fileName} 생성 완료`);
  }
}

generateTsv({ fileCount: 100, processCount: 30 });
