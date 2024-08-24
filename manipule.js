const { spawn } = require('child_process');

// Caminho para o executável
const executablePath = './combine_files.exe'; // Ajuste o caminho conforme necessário

// Nome do arquivo de saída (pode ser passado como argumento ou definido aqui)
const outputFile = 'combined_output.jpg';

// Função para executar o programa e mostrar o status
function runExecutable() {
    // Executa o programa com o argumento especificado
    const process = spawn(executablePath, [outputFile]);

    // Variável para armazenar dados acumulados
    let outputData = '';

    // Captura a saída padrão (stdout)
    process.stdout.on('data', (data) => {
        outputData += data.toString();
        
        // Tenta analisar o JSON
        try {
            const json = JSON.parse(outputData);
            console.log(`Progress: ${json.progress}, Size: ${json.size}`);
        } catch (e) {
            // Se não for JSON válido, continue acumulando dados
        }
    });

    // Captura a saída de erro (stderr)
    process.stderr.on('data', (data) => {
        console.error(`stderr: ${data}`);
    });

    // Captura o evento de encerramento do processo
    process.on('close', (code) => {
        console.log(`Process exited with code ${code}`);
    });
}

// Chama a função para executar o programa
runExecutable();
