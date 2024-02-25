VCPKG_HOME	=	/opt/vcpkg

dev:
	docker compose run -p 8080:8080 --rm app

clean:
	rm -rf target

build:
	cmake -B ./target -S . "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake"
	cd target && make && cd ..
