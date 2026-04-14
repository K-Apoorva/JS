async function placeOrder(products, order) {
    if (!order.productId || !order.quantity) {
        throw new Error("Invalid order data");
    }

    const product = products.find(p => p.id === order.productId);

    if (!product) {
        throw new Error("Product not found");
    }

    if (product.stock < order.quantity) {
        throw new Error("Insufficient stock");
    }

    product.stock -= order.quantity;

    const newOrder = {
        orderId: Date.now(),
        productId: order.productId,
        quantity: order.quantity,
        status: "confirmed"
    };

    return newOrder;
}

let products = [
    { id: 101, name: "Laptop", stock: 5 }
];

placeOrder(products, {
    productId: 101,
    quantity: 2
}).then(order => {
    console.log("Order placed:", order.orderId);
}).catch(err => {
    console.error(err.message);
});
