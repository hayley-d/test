document.getElementById('queryForm').addEventListener('submit', async (event) =>{
    event.preventDefault();



    const query = document.getElementById('inputData').value;

    if(!query){
        document.getElementById('errorMessage').textContent = 'Please enter a query';
        return;
    }

    //convert the query to json
    const queryJson = {query: query};
    
    console.log('JSON Data:',queryJson);    //debugging

    try{
        const response = await fetch('http://localhost:3000/QRY',{

            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(queryJson),
        });

        if(!response.ok){
            const errorData = await response.json();
            throw new Error(errorData.error || 'Query failed');
        }

        const data = await response.json();
        console.log('Query successfully processed:', data);
    }

    catch(error){
        console.error('Error during query:', error);
        document.getElementById('errorMessage').textContent = error.message;
    }


});