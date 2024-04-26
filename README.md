# sqscpp

*Disclaimer* - it's a for fun project - primarily to dust-off C++ skills.

C++ (partial) implementation of an in-memory AWS SQS.

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
