VCPKG_HOME	=	/opt/vcpkg

dev:
	docker compose run -p 8080:8080 --rm app

clean:
	rm -rf target

run_elasticmq:
	docker run -p 9324:9324 -p 9325:9325 softwaremill/elasticmq-native

run_build:
	cmake -B ./target -S . "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake" -DBoost_NO_WARN_NEW_VERSIONS=1
	cd target && make && cd ..

run_test: run_build
	cd target && ctest --output-on-failure && cd ..

run_fmt:
	find src -regex '.*\.\(cpp\|hpp\)' | xargs clang-format -style=Google -i
