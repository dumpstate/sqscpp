const { SendMessageCommand, SQSClient } = require("@aws-sdk/client-sqs")

const client = new SQSClient({
    region: "us-west-2",
    endpoint: "http://localhost:8080/",
    credentials: {
        accessKeyId: "access-key-id",
        secretAccessKey: "secret-access-key",
    },
})
const SQS_QUEUE_URL = "http://0.0.0.0:8080/000000000000/foo"

async function main() {
    const n = parseInt(process.argv[2]) || 1;
    for (let i = 0; i < n; i++) {
        const command = new SendMessageCommand({
            QueueUrl: SQS_QUEUE_URL,
            MessageBody: `Hello, World! ${i + 1} / ${n}`,
        })
        const response = await client.send(command)
        console.log(response)
    }
}

main()
