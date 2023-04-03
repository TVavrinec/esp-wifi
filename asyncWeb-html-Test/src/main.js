function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
    else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
    xhr.send();
}

const file = new File(['foo'], 'new-note.txt', {
type: 'text/plain',
})

// function download() {
const link = document.createElement('a')
const url = URL.createObjectURL(file)

link.href = url
link.download = file.name
document.body.appendChild(link)
link.click()

document.body.removeChild(link)
window.URL.revokeObjectURL(url)
// }