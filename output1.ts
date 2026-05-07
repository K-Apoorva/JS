type User = {
    id: number;
    email: string;
    password: string;
    isActive: boolean;
};

async function registerUser(
    users: User[],
    userData: UserInput
): Promise<User> {
    if (!userData.email || !userData.password) {
        throw new Error("Invalid input");
    }
    const existingUser = users.find((u: User) => u.email === userData.email);
    if (existingUser) {
        throw new Error("User already exists");
    }
    const newUser: User = {
        id: Date.now(),
        email: userData.email,
        password: userData.password,
        isActive: true
    };
    users.push(newUser);
    return newUser;
}

let users: User[] = [];
