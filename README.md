# url-shortener

This project is url shortener.
To launch, you need to have `docker compose` installed in your system.
After installing, execute:
`POSTGRES_USER=postgres POSTGRES_PASSWORD=root docker compose up --build -d`
while being in repository root directory.

After successfully launching the project, go to the `http://127.0.0.1:54321:` in your browser and enjoy!
You can check http://127.0.0.1:21377 which is pgadmin's website.
Enter login `pg_admin_address@gmail.com`
and password `dont tell anyone`.
Add postgres server, remember to use `172.26.1.2` address and the credentials entered earlier.


Front-end of this project is super simple, my main goal here was to show good C++ code, I know very little about js and html.

If you want to launch tests, execute
`docker exec url-shortener-web-server-1 /url-shortener/build/bin/unit_tests`
and
`docker exec url-shortener-web-server-1 /url-shortener/build/bin/integration_tests`
to make sure that everything works as intended :)
Integration tests use real database and credentials there are hard coded (postgres:root),
so make sure to use those if you want your integration tests to work.

If this was to be deployed in real world, I would add some reverse-proxy with some cert, to encrypt the communication.
