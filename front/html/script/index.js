const add_button = document.getElementById('addProduct');
const rem_button = document.getElementById('remProduct');
const html_tablecols = document.getElementById('tableCols');

const capi_url = 'http://127.0.0.1:8080';

function utils_capitalize(w = '')
{
    let y = new String();

    y += w[0].toUpperCase();
    for (let i = 1; i < w.length; i++) {
        if (w[i].charCodeAt(0) >= 65 && w[i].charCodeAt(0) < 97) {
            y+= ' ';
        }
        y += w[i];
    }

    return (y);
}

const xhr = new XMLHttpRequest();
xhr.onreadystatechange = () => {
    if (xhr.readyState === XMLHttpRequest.DONE) {
        const status = xhr.status;
        if (status === 0 || (status >= 200 || status < 400)) {
            // const data = JSON.parse(xhr.responseText);
            console.log(xhr.responseText);
        }
    }
};

xhr.open("GET", capi_url, true);
xhr.send();

add_button.addEventListener('click', () => {
    console.log('Add button clicked.');
});

rem_button.addEventListener('click', () => {
    console.log('Remove button clicked.');
});
