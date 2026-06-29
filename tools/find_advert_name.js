const crypto = require('crypto');

// Utilizamos o SHA-256 e Little Endian (igual ao ESP32/nRF52) para o cálculo do hash
function calculateNodeHash(nodeName, byte0, byte1, byte2, byte3) {
	const nameBuffer = Buffer.from(nodeName, 'utf8');
	const idBuffer = Buffer.from([byte0, byte1, byte2, byte3]);
	const combinedBuffer = Buffer.concat([nameBuffer, idBuffer]);

	const hashResult = crypto.createHash('sha256').update(combinedBuffer).digest();

	// Ler os primeiros 4 bytes do hash em formato Little Endian (UInt32)
	return hashResult.readUInt32LE(0);
}

function parseNodeIdBytes(nodeId) {
	if (nodeId.length < 8) {
		throw new Error("Node ID tem de ter pelo menos 8 caracteres hex para os 4 bytes (ex: 877774ff).");
	}
	return {
		byte0: parseInt(nodeId.substring(0, 2), 16),
		byte1: parseInt(nodeId.substring(2, 4), 16),
		byte2: parseInt(nodeId.substring(4, 6), 16),
		byte3: parseInt(nodeId.substring(6, 8), 16)
	};
}

// A janela no LusoFW com as smart-adverts nas simulações finais é de 23 horas!
const WINDOW_SECONDS = 23 * 3600;

function estimateNextAdvertEpoch(nodeName, nodeId, nowEpoch) {
	const { byte0, byte1, byte2, byte3 } = parseNodeIdBytes(nodeId);
	const hash = calculateNodeHash(nodeName, byte0, byte1, byte2, byte3);
	const offset = hash % WINDOW_SECONDS;

	let current_cycle_start = nowEpoch - (nowEpoch % WINDOW_SECONDS);
	let target = current_cycle_start + offset;

	// Se o advert do ciclo atual já passou (considerando a margem de jitter negativo de 3s)
	if (nowEpoch + 3 >= target) {
		current_cycle_start += WINDOW_SECONDS;
		target += WINDOW_SECONDS;
	}

	const jitter = ((hash ^ current_cycle_start) % 7) - 3;
	return target + jitter;
}

function bruteForceName(nodeId, minTargetEpoch, maxTargetEpoch, nowEpoch) {
	const chars = '0123456789abcdefghijklmnopqrstuvwxyz';
	let attempts = 0;

	console.log(`\nProcurando um nome para o Node ID '${nodeId}'...`);
	const dMin = new Date(minTargetEpoch * 1000).toISOString().replace('T', ' ').substring(0, 19);
	const dMax = new Date(maxTargetEpoch * 1000).toISOString().replace('T', ' ').substring(0, 19);
	console.log(`O advert deve calhar entre ${dMin} e ${dMax} UTC.`);

	while (true) {
		// Gerar 6 caracteres aleatórios
		let randomStr = '';
		for (let i = 0; i < 6; i++) {
			randomStr += chars[Math.floor(Math.random() * chars.length)];
		}

		// Embutir no formato exigido
		const name = `Testing ${randomStr}`;
		const timeEpoch = estimateNextAdvertEpoch(name, nodeId, nowEpoch);

		if (timeEpoch >= minTargetEpoch && timeEpoch <= maxTargetEpoch) {
			console.log(`\n=> SUCESSO após ${attempts} tentativas!`);
			console.log(`=> set name ${name}`);
			const dTarget = new Date(timeEpoch * 1000).toISOString().replace('T', ' ').substring(0, 19);
			console.log(`=> Hora estimada do Advert: ${dTarget} UTC\n`);
			return name;
		}

		attempts++;
		if (attempts > 5000000) {
			console.log("\nDemasiadas tentativas (mais de 5 milhões). A abortar.");
			break;
		}
	}
}

// Lógica de arranque do script
const args = process.argv.slice(2);
if (args.length < 1) {
	console.log("Uso: node find_advert_name.js <node_id_4_bytes>");
	console.log("Exemplo: node find_advert_name.js 877774ff");
	process.exit(1);
}

const nodeId = args[0];

// Calcula o Unix Epoch atual em segundos
const nowEpoch = Math.floor(Date.now() / 1000);

// Janela alvo: Entre 2 e 5 minutos a partir de AGORA
const minTargetEpoch = nowEpoch + 120;
const maxTargetEpoch = nowEpoch + 300;

bruteForceName(nodeId, minTargetEpoch, maxTargetEpoch, nowEpoch);
