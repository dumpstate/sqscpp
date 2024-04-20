const {
    ReceiveMessageCommand,
    DeleteMessageCommand,
    SQSClient
} = require("@aws-sdk/client-sqs")

const client = new SQSClient({
    region: "us-west-2",
    endpoint: "http://localhost:8080/",
    credentials: {
        accessKeyId: "access-key-id",
        secretAccessKey: "secret-access-key",
    },
})
const SQS_QUEUE_URL = "http://0.0.0.0:8080/000000000000/foo"
const delay = (ms) => new Promise((resolve) => setTimeout(resolve, ms))

async function main() {
    while(true) {
        const { Messages } = await client.send(
            new ReceiveMessageCommand({
                MaxNumberOfMessages: 2,
                QueueUrl: SQS_QUEUE_URL,
            }),
        )

        if (!Messages || Messages.length === 0) {
            await delay(1000)
            continue;
        }

        console.log(Messages)
        // simulate processing
        await delay(500)

        for (const message of Messages) {
            // acknowledge
            await client.send(
                new DeleteMessageCommand({
                    QueueUrl: SQS_QUEUE_URL,
                    ReceiptHandle: message.ReceiptHandle,
                }),
            )
        }
    }
}

main()
