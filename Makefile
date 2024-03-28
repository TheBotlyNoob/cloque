upload_conn_serial:
	make compiledb
	make upload
	make serial

compiledb:
	pio run -t compiledb

serial:
	pio device monitor

upload:
	pio run -t upload

clean:
	pio run -t clean
