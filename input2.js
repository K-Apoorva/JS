async function uploadFile(files, fileData) {
    if (!fileData.name || !fileData.size) {
        throw new Error("Invalid file data");
    }

    const MAX_SIZE = 5 * 1024 * 1024; // 5MB

    if (fileData.size > MAX_SIZE) {
        throw new Error("File size exceeds limit");
    }

    const fileExists = files.find(f => f.name === fileData.name);

    if (fileExists) {
        throw new Error("File already exists");
    }

    const newFile = {
        id: Date.now(),
        name: fileData.name,
        size: fileData.size,
        uploadedAt: new Date().toISOString()
    };

    files.push(newFile);

    return newFile;
}

let files = [];

uploadFile(files, {
    name: "resume.pdf",
    size: 1024 * 500
}).then(file => {
    console.log("Uploaded:", file.name);
}).catch(err => {
    console.error(err.message);
});
