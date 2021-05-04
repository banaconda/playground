var config = {
    type: Phaser.AUTO,
    width: 800,
    height: 600,
    parent: 'main-screen',
    // physics: {
    //     default: 'arcade',
    //     arcade: {
    //         gravity: { y: 100 },
    //         debug: false
    //     }
    // },
    scene: {
        preload: preload,
        create: create,
        update: update
    }
};
var key;
var mouse;
var game = new Phaser.Game(config)

var obj_list = []
var obj_num = 1;

var text;
var socket = new WebSocket("ws://" + window.location.host + "/echo/" + title);

var count = 0;
socket.onopen = function () {
};

socket.onmessage = function (e) {
    console.log("Server: " + e.data + "\n");
    text.text = "messgae: " + count;
    count++
};

function send() {
    socket.send(input.value);
    input.value = "";
}

function preload() {
    for (var i = 0; i < obj_num; i++) {
        var obj = this.add.rectangle(400, 300, 10, 10, 0x9393ff);
        obj_list.push(obj)
    }
    text = this.add.text(0, 0,
        title, { fontFamily: 'Arial', fontSize: 20, color: '#00ff00' });
}

function create() {
    key = this.input.keyboard.createCursorKeys();

    this.input.mouse.disableContextMenu();
    this.input.on('pointerdown', function (pointer){
        var obj = this.scene.add.rectangle(pointer.x, pointer.y, 10, 10, 0x9393ff);
        obj_list.push(obj)
        obj_num++;
    });
}

var count = 0;
function update() {
    // if (this.input.activePointer.isDown){
    //     var obj = this.add.rectangle(this.input.activePointer.worldX, this.input.activePointer.worldY, 10, 10, 0x9393ff)
    //     obj_list.push(obj)
    //     obj_num++;
    // }

    for (var i = 0; i < obj_num; i++){
        obj_list[i].x += Math.random() * 2 - 1
        obj_list[i].y += Math.random() * 2 - 1
    }

    var pointer = this.input.activePointer;
    text.setText([
        'x: ' + pointer.x,
        'y: ' + pointer.y,
        'isDown: ' + pointer.isDown,
        'rightButtonDown: ' + pointer.rightButtonDown()
    ]);
}
