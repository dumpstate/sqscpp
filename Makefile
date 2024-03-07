VCPKG_HOME	=	/opt/vcpkg

dev:
	docker compose run -p 8080:8080 --rm app

clean:
	rm -rf target

run_build:
	cmake -B ./target -S . "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake" -DBoost_NO_WARN_NEW_VERSIONS=1
	cd target && make && cd ..

run_test: run_build
	cd target && ctest && cd ..
