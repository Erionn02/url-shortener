document.getElementById('shortenForm').addEventListener('submit', function (event) {
    event.preventDefault();

    const url = document.getElementById('url').value;
    const customPath = document.getElementById('customPath').value;
    const resultDiv = document.getElementById('result');
    const errorDiv = document.getElementById('error');
    const shortenedUrl = document.getElementById('shortenedUrl');
    const errorMessage = document.getElementById('errorMessage');

    resultDiv.classList.add('hidden');
    errorDiv.classList.add('hidden');

    fetch('/shorten_url', {
        method: 'POST',
        headers: {
            'url_to_shorten': url,
            ...(customPath && { 'custom_path': customPath })
        }
    })
        .then(response => {
            return response.text().then(text => {
                return { status: response.status, body: text };
            });
        })
        .then(result => {
            if (result.status === 200) {
                shortenedUrl.textContent = result.body;
                resultDiv.classList.remove('hidden');
            } else {
                errorMessage.textContent = result.body;
                errorDiv.classList.remove('hidden');
            }
        })
        .catch(error => {
            errorMessage.textContent = error.toString();
            errorDiv.classList.remove('hidden');
        });
});
