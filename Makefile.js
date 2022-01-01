const fs = require("fs");
const util = require('util');
const exec = util.promisify(require('child_process').exec);

const readRecursiveFiles = path => {
    const files = fs.readdirSync(path);
    const result = [];

    files.forEach(file => {
        const filePath = `${path}/${file}`;
        if (fs.statSync(filePath).isDirectory()) {
            result.push(...readRecursiveFiles(filePath));
        } else {
            result.push(filePath);
        }
    });

    return result;
}

const readFolders = path => {
    const files = fs.readdirSync(path);
    const result = [];

    files.forEach(file => {
        const filePath = `${path}/${file}`;
        if (fs.statSync(filePath).isDirectory()) {
            result.push(filePath);
        }
    });

    return result;
}

const readRecursiveFolders = path => {
    const files = fs.readdirSync(path);
    const result = [];

    files.forEach(file => {
        const filePath = `${path}/${file}`;
        if (fs.statSync(filePath).isDirectory()) {
            result.push(filePath);
            result.push(...readRecursiveFolders(filePath));
        }
    });

    return result;
}

const runCommand = async command => {
    const { stdout, stderr, error } = await exec(command);
    if (stderr) console.error(`stderr: ${stderr}`);
    if (error) console.error(`error: ${error}`);

    return stdout;
}

const asmCompile = async (file, output) => {
    await runCommand("cd src/kernel/assembly");
    const result = await runCommand(`nasm -f elf64 ${file} -o ${output}`);
    
    console.log(result);
    await runCommand("cd ../../..");
}

(async () => {
    var cmdHeap = "x86_64-elf-ld -n -o dist/kernel.bin -T targets/linker.ld";

    await runCommand("mkdir build").catch(() => {});
    await runCommand("mkdir dist").catch(() => {});

    await readFolders("./src/").forEach(async folder => {
        await runCommand(`mkdir ${folder.replace("./src/", "build/")}`).catch(() => {});
    });

    await readRecursiveFolders("./src").forEach(async folder => {
        await runCommand(`mkdir ${folder.replace("./src/", "build/")}`).catch(() => {});
    });

	await runCommand("rm -rf build/include");

    await readRecursiveFiles("./src").forEach(async file => {
        if (file.endsWith(".c")) {
            const result = await runCommand(`x86_64-elf-gcc -ffreestanding -c ${file.replace("./src", "src")} -o ${file.replace("./src", "build").replace(".c", ".o")}`);
            console.log(result);
        } else if (file.endsWith(".asm")) {
            await asmCompile(file.replace("./src/kernel/assembly/", ""), file.replace("./src", "../../../build").replace(".asm", ".o"));
        }
    });
    
    await readRecursiveFiles("./build").forEach(async file => {
        cmdHeap += ` ${file.replace("./build/", "build/")}`;
    });

    await runCommand(cmdHeap).catch(() => {});
    await runCommand("cp dist/kernel.bin targets/iso/boot/kernel.bin").catch(() => {});
    await runCommand("grub-mkrescue /usr/lib/grub/i386-pc -o dist/kernel.iso targets/iso").catch(() => {});

	await runCommand("rm -rf build").catch(() => {});
    await runCommand("rm -rf dist/kernel.bin").catch(() => {});
})();