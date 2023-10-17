const axios = require('axios');

async function rpcRequest(method, params) {
    try {
        const response = await axios.post('http://localhost:8080', {
            jsonrpc: '2.0',
            method: method,
            params: params,
            id: 1
        });

        if (response.data.error) {
            throw new Error(response.data.error.message);
        }

        return response.data.result;
    } catch (error) {
        console.error('RPC request failed:', error.message);
        return null;
    }
}

(async () => {
    const sumResult = await rpcRequest('add', [5, 3]);
    console.log('Result from RPC server (add):', sumResult);

    const multiplyResult = await rpcRequest('multiply', [4, 2]);
    console.log('Result from RPC server (multiply):', multiplyResult);
})();
