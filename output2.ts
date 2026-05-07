type FileRecord = {
    id: number;
    name: string;
    size: number;
    uploadedAt: string;
};

async function uploadFile(
    files: FileRecord[],
    fileData: FileInput
): Promise<FileRecord> {
    if (!fileData.name || !fileData.size) {
        throw new Error("Invalid file data");
    }
    const MAX_SIZE: number = 5 * 1024 * 1024;
    if (fileData.size > MAX_SIZE) {
        throw new Error("File size exceeds limit");
    }
    const fileExists = files.find((f: FileRecord) => f.name === fileData.name);
    if (fileExists) {
        throw new Error("File already exists");
    }
    const newFile: FileRecord = {
        id: Date.now(),
        name: fileData.name,
        size: fileData.size,
        uploadedAt: new Date().toISOString()
    };
    files.push(newFile);
    return newFile;
}

let files: FileRecord[] = [];
