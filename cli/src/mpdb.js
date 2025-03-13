/*
 * NOTE: 
 *  - Authentication required for log in. Use of AUTH command. Users username and password stored in a textfile (users.txt). - TEMPORARY
 *  - Server secret stored in textfile (ServerSecret.txt) - TEMPORARY
 *  - Splits user input into words to prepare validation
 *  - Validation for GET SET UPDATE DELETE 
 *  - If valid, send original query to server
 *  - Output server response
 *  - Current colours in terminal aren't the greatest, could use some work for aesthetics.
 *
 * Run: node mpdb.js
 */

// imports
// const readline = require('readline');
import readline from 'readline';
// const figlet = require("figlet"); 
import figlet from 'figlet'; //for the cool art
// const fs = require('fs'); 
import fs from 'fs'; //to read files
// const kleur = require('kleur'); 
import kleur from 'kleur'; //for nice colours


// HELPER FUNCTIONS 

//for quoted strings
function tokenise(input) {

    //double "[^"]*   & single '[^']*'    quoted strings or sequence of non-whitespace \S+
    const pattern = /"[^"]*"|'[^']*'|\S+/g;

    // find all matches or empty array if no matches 
    let tokens = input.match(pattern) || [];

    // remove surrounding quotes
    // tokens = tokens.map(token => {
    //     if ((token.startsWith('"') && token.endsWith('"')) || (token.startsWith("'") && token.endsWith("'"))) {
    //         return token.slice(1, -1);
    //     }
    //     return token;
    // });

    return tokens;
}

// splits user input
function splitBySpace(input) {
    return tokenise(input);
}

function getSyntax(words) {

    // query needs at least command and collection
    if (words.length < 1) {
        return { error: kleur.red("[ERROR] GET syntax: missing <collection>.") };
        // return { error: "[ERROR] GET syntax: missing <collection>." };
    }

    const collection = words[0];

    // optional filters
    let key = "";
    let operator = "";
    let val = "";
    let sort = "";
    let limit = -1;
    let offset = -1;

    // start at second index, ignoring the command and the collection
    let idx = 1;

    // check if word after collection is a key
    // e.g., GET PIRATES ship.name
    if (idx < words.length) {
        const possibleKey = words[idx];
        // if possible key is a number, operator, or matches that of a sort, it's not a key
        if (!isOperator(possibleKey) && !isSortToken(possibleKey) && !isNumeric(possibleKey)) {
            key = possibleKey;
            idx++;
        }
    }

    // check if we have an operator and value after the key 
    // e.g., GET PIRATES ship.type == 'Man of War'
    if (idx < words.length && isOperator(words[idx])) {
        const possibleOperator = words[idx];
        // operator but no key
        if (!key) {
            return { error: kleur.red("[ERROR] GET syntax: found operator " + possibleOperator + " but no <key> before it.") };
            // return { error: "[ERROR] GET syntax: found operator " + possibleOperator + " but no <key> before it." };
        }

        // now there's a key and an operator
        operator = possibleOperator;
        idx++;

        // next word should be the value
        if (idx >= words.length) {
            return { error: kleur.red("[ERROR] GET syntax: operator was provided but no <value> found.") };
            // return { error: "[ERROR] GET syntax: operator was provided but no <value> found." };
        }
        val = words[idx];
        idx++;
    }

    // check for sort
    // e.g., GET PIRATES ship.type == 'Frigate' DESC
    if (idx < words.length) {
        const possibleSort = words[idx].toUpperCase();
        if (possibleSort === "ASC" || possibleSort === "DESC") {
            sort = possibleSort;
            idx++;
        }
    }

    // check for limit - must be numeric
    // e.g., GET PIRATES ship.type == 'Frigate' ASC 2
    if (idx < words.length) {
        if (isNumeric(words[idx])) {
            limit = parseInt(words[idx], 10);
            idx++;
        }
    }

    // check for offset - must also be numeric
    if (idx < words.length) {
        if (isNumeric(words[idx])) {
            offset = parseInt(words[idx], 10);
            idx++;
        }
    }

    // if there are more words in the query, then its invalid syntax
    if (idx < words.length) {
        return { error: kleur.red("[ERROR] GET syntax has too many/unrecognized tokens.") };
        // return { error: "[ERROR] GET syntax has too many/unrecognized tokens." };
    }

    // operator exists but no key or value
    if (operator && (!key || !val)) {
        return { error: kleur.red("[ERROR] GET syntax: operator was specified but missing key or value.") };
        // return { error: "[ERROR] GET syntax: operator was specified but missing key or value." };
    }

    // value present but operator not
    if (val && !operator) {
        return { error: kleur.red("[ERROR] GET syntax: a value was found but no operator was specified.") };
        // return { error: "[ERROR] GET syntax: a value was found but no operator was specified." };
    }

    return {
        error: null,
        collection,
        key,
        operator,
        value: val,
        sort,
        limit,
        offset
    };
}

function isOperator(token) {
    const ops = ["<", ">", "<=", ">=", "==", "!=", "*"];
    return ops.includes(token);
}

function isSortToken(token) {
    const upper = token.toUpperCase();
    return (upper === "ASC" || upper === "DESC");
}

function isNumeric(token) {
    return !isNaN(token) && !isNaN(parseFloat(token));
}


// load users from users.txt
let users = {};
try {
    const data = fs.readFileSync('users.txt', 'utf8');
    const lines = data.split('\n');
    lines.forEach(line => {
        line = line.trim();
        if (!line) return;
        const parts = line.split(':');
        if (parts.length >= 2) {
            const username = parts[0].trim();
            const password = parts[1].trim();
            users[username] = password;
        }
    });
} catch (err) {
    console.error(kleur.red("[ERROR] Could not read users file:"), err.message);
    // console.error("[ERROR] Could not read users file:", err.message);
}

// load server secret, currently just uses a textfile
let serverSecret = "";
try {
    serverSecret = fs.readFileSync('ServerSecret.txt', 'utf8').trim();
} catch (err) {
    console.error(kleur.red("[ERROR] Could not read server secret file:"), err.message);
    // console.error("[ERROR] Could not read server secret file:", err.message);
}

// authentication variables
let isAuthenticated = false;
let currentUser = '';

// note that the currentUser refers to the username logged in using AUTH
// serverSecret is read from a textfile 
// query is the original query which the user prompted
async function sendQueryToServer(queryString) {
    const endpoint = "http://localhost:8080/QRY/";

    const bodyPayload = {
        userID: currentUser,
        server_secret: serverSecret,
        query: queryString
    };

    try {
        const response = await fetch(endpoint, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(bodyPayload)
        });

        // wait for JSON response
        const result = await response.json();

        // server output
        if (result.success) {
            console.log(kleur.green("[SERVER] success:"), JSON.stringify(result.data, null, 2));
            // console.log("[SERVER] success:", JSON.stringify(result.data, null, 2));
        } else {
            console.log(kleur.red("[SERVER] error:"), result.message);
            // console.log("[SERVER] error:", result.message);
        }

    } catch (err) {
        // if complete error
        console.log(kleur.red("[ERROR] Could not send query to C++ server:"), err.message);
        // console.log("[ERROR] Could not send query to C++ server:", err.message);
    }
}

// handle reading and writing
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    prompt: '\nmpdb> '
});


// hide password
rl._writeToOutput = function (stringToWrite) {
    if (rl.stdoutMuted)
        rl.output.write("*");
    else
        rl.output.write(stringToWrite);
};

console.log(
    kleur.cyan(figlet.textSync("MPDB CLI", { horizontalLayout: "full" }))
    // figlet.textSync("MPDB CLI", { horizontalLayout: "full" })
);

console.log(kleur.cyan("Welcome to mpdb (NoSQL CLI)"));
// console.log("Welcome to mpdb (NoSQL CLI)");
console.log("Please authenticate using: AUTH <username>");
console.log("Type EXIT or QUIT to quit.\n");


rl.prompt();

// event listener to handle each line of  input
rl.on('line', (input) => {
    // ignore empty
    if (!input.trim()) {
        rl.prompt();
        return;
    }

    // split input
    const words = splitBySpace(input);
    const command = words[0].toUpperCase();

    if (input.trim().toUpperCase() === "EXIT" || input.trim().toUpperCase() === "QUIT") {
        //console.log("bye bye!");
        return rl.close();
    }

    // check if user is authenticated
    if (!isAuthenticated) {
        //some validation
        if (command === "AUTH") {
            if (words.length < 2) {
                console.log(kleur.red("[ERROR] AUTH command requires a username."));
                // console.log("[ERROR] AUTH command requires a username.");
                rl.prompt();
                return;
            }
            const username = words[1];
            if (!users.hasOwnProperty(username)) {
                console.log(kleur.red("[ERROR] Unknown username."));
                // console.log("[ERROR] Unknown username.");
                rl.prompt();
                return;
            }
            // password time - make sure its hidden
            // rl.stdoutMuted = true;
            rl.question("Enter password: ", (password) => {
                // rl.stdoutMuted = false;
                // compare time - make sure entered password matches with text file
                if (password === users[username]) {
                    isAuthenticated = true;
                    currentUser = username;
                    console.log(kleur.green("\n[INFO] Authentication successful. Welcome, " + username + "."));
                    // console.log("\n[INFO] Authentication successful. Welcome, " + username + ".");
                } else {
                    console.log(kleur.red("\n[ERROR] Incorrect password."));
                    // console.log("\n[ERROR] Incorrect password.");
                }
                rl.prompt();
            });
            return;
        } else {
            console.log(kleur.red("[ERROR] Please login first. Use AUTH <username> to login."));
            // console.log("[ERROR] Please login first. Use AUTH <username> to login.");
            rl.prompt();
            return;
        }
    }

    // authentication successful, can now proceed.



    // GET command
    // Format: GET <collection> <key> <operator> <value> <sort> <limit> <offset>
    else if (command === "GET") {

        // original query to pass if validation succeeds 
        const originalQuery = input;

        const remainingQ = words.slice(1);

        // use helper to validate
        const parsed = getSyntax(remainingQ);

        if (parsed.error) {
            // invalid get query - print error
            console.log(parsed.error);
            rl.prompt();
            return;
        }

        // query syntax is valid
        // TEMPORARY OUTPUT
        console.log(kleur.green("[INFO] GET Command:"));
        // console.log("[INFO] GET Command:");
        console.log("  Collection:", parsed.collection);

        if (parsed.key) {
            if (parsed.operator && parsed.value) {
                console.log("  Filter:", parsed.key, parsed.operator, parsed.value);
            } else {
                console.log("  Key:", parsed.key);
            }
        }

        if (parsed.sort) {
            console.log("  Sort:", parsed.sort);
        }

        if (parsed.limit !== -1) {
            console.log("  Limit:", parsed.limit);
        }

        if (parsed.offset !== -1) {
            console.log("  Offset:", parsed.offset);
        }

        // send query to server
        sendQueryToServer(originalQuery)
            .then(() => rl.prompt())
            .catch(() => rl.prompt());
        return;

        // rl.prompt();
    }

    // SET command
    // Format: SET <collection> <data>
    else if (command === "SET") {

        const originalQuery = input;

        if (words.length < 3) {
            console.log(kleur.red("[ERROR] SET requires a <collection> and <data>."));
            // console.log("[ERROR] SET requires a <collection> and <data>.");
            rl.prompt();
            return;
        }

        const collection = words[1];
        const tokens = words.slice(2);
        const data = tokens.join(" ");

        // ensure data portion is enclosed with single quotes
        if (!data.startsWith("'") || !data.endsWith("'")) {
            console.log(kleur.red("[ERROR] JSON data must be enclosed in single quotes."));
            // console.log("[ERROR] JSON data must be enclosed in single quotes.");
            rl.prompt();
            return;
        }

        // strip single quotes for validation
        const jsonString = data.slice(1, -1).trim();

        // validate JSON
        try {
            // parse as JSON
            const jsonData = JSON.parse(jsonString);
            // success
            console.log(kleur.green("[INFO] SET Command:"));
            // console.log("[INFO] SET Command:");
            console.log("  Collection:", collection);
            console.log("  Parsed Data:", jsonData);

            sendQueryToServer(originalQuery)
            .then(() => rl.prompt())
            .catch(() => rl.prompt());

            return;

        } catch (err) {
            // invalid JSON
            console.log(kleur.red("[ERROR] Invalid JSON data for SET command:"), err.message);
            // console.log("[ERROR] Invalid JSON data for SET command:", err.message);
        }

        rl.prompt();

    }

    // UPDATE command
    // Format: UPDATE <collection> <update_key> <update_val> <GET_QUERY>
    else if (command === "UPDATE") {

        const originalQuery = input;

        // minimum word count requirement
        if (words.length < 5) {
            console.log(kleur.red("[ERROR] UPDATE requires: UPDATE <collection> <update_key> <update_val> GET <GET_query>"));
            // console.log("[ERROR] UPDATE requires: UPDATE <collection> <update_key> <update_val> GET <GET_query>");
            rl.prompt();
            return;
        }

        const updateCollection = words[1];
        const updateKey = words[2];
        const updateVal = words[3];
        const maybeGET = words[4].toUpperCase();

        // required GET must be present
        if (maybeGET !== "GET") {
            console.log(kleur.red("[ERROR] UPDATE syntax: missing 'GET' before the sub-query."));
            // console.log("[ERROR] UPDATE syntax: missing 'GET' before the sub-query.");
            rl.prompt();
            return;
        }

        // the rest should be the GET query
        const subQuery = words.slice(5);

        // validate get syntax
        const parsed = getSyntax(subQuery);
        if (parsed.error) {
            // if fail show error
            console.log(parsed.error);
            rl.prompt();
            return;
        }

        // if we get here - syntax is valid
        console.log(kleur.green("[INFO] UPDATE Command:"));
        // console.log("[INFO] UPDATE Command:");
        console.log("  Collection:", updateCollection);
        console.log("  Update Key:", updateKey);
        console.log("  Update Val:", updateVal);

        console.log("  GET Sub-query:");
        console.log("    Collection:", parsed.collection);
        if (parsed.key) {
            if (parsed.operator && parsed.value) {
                console.log("    Filter:", parsed.key, parsed.operator, parsed.value);
            } else {
                console.log("    Key:", parsed.key);
            }
        }
        if (parsed.sort) {
            console.log("    Sort:", parsed.sort);
        }
        if (parsed.limit !== -1) {
            console.log("    Limit:", parsed.limit);
        }
        if (parsed.offset !== -1) {
            console.log("    Offset:", parsed.offset);
        }


        sendQueryToServer(originalQuery)
        .then(() => rl.prompt())
        .catch(() => rl.prompt());
        return;

        // rl.prompt();
    }

    // DELETE command
    // Format: DELETE <delete_key> <GET_QUERY>
    else if (command === "DELETE") {

        const originalQuery = input;

        // check if query has minimum requirements
        if (words.length < 4) {
            console.log(kleur.red("[ERROR] DELETE syntax: DELETE <delete_key> GET <collection> [filters ...]"));
            // console.log("[ERROR] DELETE syntax: DELETE <delete_key> GET <collection> [filters ...]");
            rl.prompt();
            return;
        }

        const deleteKey = words[1]; // can be '*' or any key
        const maybeGET = words[2].toUpperCase();

        // next word should be GET command
        if (maybeGET !== "GET") {
            console.log(kleur.red("[ERROR] DELETE syntax: expected 'GET' after <delete_key>."));
            // console.log("[ERROR] DELETE syntax: expected 'GET' after <delete_key>.");
            rl.prompt();
            return;
        }

        // the rest should be the GET query
        const getPart = words.slice(3);
        const parsedGet = getSyntax(getPart);

        if (parsedGet.error) {
            // fail - print error
            console.log(parsedGet.error);
            rl.prompt();
            return;
        }

        // if we reach here - valid DELETE syntax
        console.log(kleur.green("[INFO] DELETE Command:"));
        // console.log("[INFO] DELETE Command:");
        console.log("  Delete Key:", deleteKey);
        console.log("  GET Sub-query:");
        console.log("    Collection:", parsedGet.collection);

        if (parsedGet.key) {
            if (parsedGet.operator && parsedGet.value) {
                console.log("    Filter:", parsedGet.key, parsedGet.operator, parsedGet.value);
            } else {
                console.log("    Key:", parsedGet.key);
            }
        }
        if (parsedGet.sort) {
            console.log("    Sort:", parsedGet.sort);
        }
        if (parsedGet.limit !== -1) {
            console.log("    Limit:", parsedGet.limit);
        }
        if (parsedGet.offset !== -1) {
            console.log("    Offset:", parsedGet.offset);
        }


        sendQueryToServer(originalQuery)
        .then(() => rl.prompt())
        .catch(() => rl.prompt());
        return;

        // rl.prompt();
    }

    //unknown command
    else {
        console.log(kleur.red("[ERROR] Unrecognized command:"), command);
        // console.log("[ERROR] Unrecognized command:", command);
    }

    rl.prompt();
});

// close 
rl.on('close', () => {
    console.log(kleur.cyan("\nBye bye"));
    // console.log("\nBye bye");
    process.exit(0);
});
