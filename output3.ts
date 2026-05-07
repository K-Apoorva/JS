type Order = {
    orderId: number;
    productId: number;
    quantity: number;
    status: string;
};

type Product = {
    id: number;
    name: string;
    stock: number;
};

async function placeOrder(
    products: Product[],
    order: OrderInput
): Promise<Order> {
    if (!order.productId || !order.quantity) {
        throw new Error("Invalid order data");
    }
    const product = products.find((p: Product) => p.id === order.productId);
    if (!product) {
        throw new Error("Product not found");
    }
    if (product.stock < order.quantity) {
        throw new Error("Insufficient stock");
    }
    product.stock -= order.quantity;
    const newOrder: Order = {
        orderId: Date.now(),
        productId: order.productId,
        quantity: order.quantity,
        status: "confirmed"
    };
    return newOrder;
}

let products: Product[] = [{
        id: 101,
        name: "Laptop",
        stock: 5
    }];
