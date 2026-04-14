async function registerUser(users, userData) {
    if (!userData.email || !userData.password) {
        throw new Error("Invalid input");
    }

    const existingUser = users.find(u => u.email === userData.email);

    if (existingUser) {
        throw new Error("User already exists");
    }

    const newUser = {
        id: Date.now(),
        email: userData.email,
        password: userData.password,
        isActive: true
    };

    users.push(newUser);

    return newUser;
}

let users = [];

registerUser(users, {
    email: "john@example.com",
    password: "123456"
}).then(user => {
    console.log("Registered:", user.email);
});
