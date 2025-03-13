Integration testing for the CLI Terminal is as follows. 

How to run the integration test:
	- run `npm test`.
	- Tests whether a valid query is successfully sent to the fake server from the terminal.
	- And if the CLI-Terminal receives the correct response from the fake server.

Required packages for the test:
	- `npm install --save-dev mocha chai express body-parser`.
	- This test makes use of Mocha and Chai.

Some notes about the integration test:
	- The entire project had to be changed from CommonJS to ES modules.
		- All imports were changed.
		- package.json was modified to specify project type.
	- Running the test while hiding the password does not currently work.
		- This is under maintenance and will hopefully be fixed.
	- All console.log's including colour has a duplicate line of code beneath which is commented out and does not include colour.
		- The ANSI colour codes was suspected to be giving problems with the integration test.

How the integration test works: 
	- `mpdb.js`, `fakeServer.js` and `mochaTest.js` work together for the test.
	- The fake server starts running and is ready to accept requests at http://localhost:8080/QRY/
	- Currently the Mocha Test tests the sending of a simple GET query and ensures a correct response from the terminal:
		- Spawns the CLI-Terminal as a child process
		- Waits for 'mpdb>' output.
		- Logs in using 'Alex' as a username.
		- Uses 'AlexPassword' for the password.
		- If authentication is successful, it prompts 'GET PIRATES' as a query.
		- If CLI's response is correct, it exits and the integration test is concluded. 
