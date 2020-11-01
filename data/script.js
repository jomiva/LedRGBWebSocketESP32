// WebSockets
var websocket;
// Simple example, see optional options for more configuration.
const pickr = Pickr.create({
    el: ".color-picker",
    theme: "monolith", // or 'monolith', or 'nano'

    components: {
        // Main components
        preview: true,
        opacity: false,
        hue: true,

        // Input / output Options
        interaction: {
            hex: true,
            rgba: false,
            hsla: false,
            hsva: false,
            cmyk: false,
            input: true,
            clear: false,
            save: false
        }
    }
});

pickr.on("change", color => {
    let rgbcolor = color.toHEXA();
    websocket.send(rgbcolor);
    console.log(rgbcolor);
});

function webSocketESP() {
    websocket = new WebSocket("ws://" + location.hostname + ":1337/");

    websocket.onopen = function(evt) {
        console.log(evt);
    };
    websocket.onclose = function(evt) {
        console.log(evt);
    };
    websocket.onmessage = function(evt) {
        console.log(evt);
    };
    websocket.onerror = function(evt) {
        console.log(evt);
    };
}

window.onload = function() {
    webSocketESP();
};