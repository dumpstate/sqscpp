# sqscpp

C++ implementation of AWS SQS.

## Local Development

Start the development docker container:

```sh
docker compose run -p 8080:8080 --rm app
```

or use the shortcut:

```sh
make dev
```

Then, from the container shell:

```sh
make run_build
```

finally, start the app:

```sh
./target/sqscpp
```
