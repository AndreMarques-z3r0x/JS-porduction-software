const openModalButtons = document.querySelectorAll('[data-modal-target]');
const closeModalButtons = document.querySelectorAll('[data-close-button]');
const overlay = document.getElementById('overlay');

openModalButtons.forEach(button => {
    button.addEventListener('click', () => {
    const modal = document.querySelector(button.dataset.modalTarget)
    openModal(modal)    
    })
})

closeModalButtons.forEach(button => {
    button.addEventListener('click', () => {
    const modal = button.closest('.modal')
    closeModal(modal)    
    })
})

function openModal(modal) {
    if (modal == null) return
    modal.classList.add('active')
    overlay.classList.add('active')
}

function closeModal(modal){
    if(modal == null) return
    modal.classList.remove('active')
    overlay.classList.remove('active')
}

let btn7d = document.getElementById('btn7d');
let btn15d = document.getElementById('btn15d');
let btn1m = document.getElementById('btn1m');
var intervalo = 0;
btn7d.addEventListener('click',() =>{
    intervalo = 7;
    btn7d.classList.add('active');
    btn15d.classList.remove('active');
    btn1m.classList.remove('active');
});
btn15d.addEventListener('click',() =>{
    intervalo = 15;
    btn15d.classList.add('active');
    btn7d.classList.remove('active');
    btn1m.classList.remove('active');
});
btn1m.addEventListener('click',() =>{
    intervalo = 1;
    btn1m.classList.add('active');
    btn15d.classList.remove('active');
    btn7d.classList.remove('active');
});


function myfunction (){
    if(intervalo == 7)
    alert("okokok it works!");
    if (intervalo == 15)
    alert("okokok i get 15");
    if (intervalo==1)
    alert("okokok i have got 1 month to live");
}


//function opendrop(n){
//    console.log(n)
//    let dropdown = document.getElementsByClassName("dropdown-container")
//    if ( dropdown[n].style.display == 'none')
//    dropdown[n].style.display = 'block'
//    else
//    dropdown[n].style.display = 'none'
//}

var dropdown = document.getElementsByClassName("dropdown-btn")
    //console.log(dropdown.length)
    for (var i = 0; i <dropdown.length;  i++) {
        dropdown[i].addEventListener('click' , function(){
        var drop = this.nextElementSibling;
        //console.log(drop)
        if (drop.style.display == 'none')
            drop.style.display = 'block'
        else
            drop.style.display = 'none'
        });
    };