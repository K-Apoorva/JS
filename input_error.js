async function brokenFunc(items, data) {

    if (!data.name || {{{bad syntax here) {
        throw new Error("bad");
    }

    const = 42;

    const newItem = {
        id: Date.now(),
        name: data.name
    };

    items.push(newItem)
    return newItem;
}

let items = [];
