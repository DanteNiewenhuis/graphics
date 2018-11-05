/* Computer Graphics, Assignment 2, Translations, Rotations and Scaling
 *
 * Description ..... Uses webGL to draw the scene.
 * Date ............ 04-11-2017
 * Created by ...... Daan Kruis
 * Original by ..... Jurgen Sturm
 *
 */
var gl;
var prog;
var width;
var height;

var frame;
var useMyTransformations;
var doRotate;
var doTranslate;
var doScale;
var speed;

var teapotVertexBuffer;
var teapotNormalBuffer;
var teapotColorBuffer;
var teapotIndexBuffer;

var cMat;
var mvMat;
var pMat;
var nMat;

function TransformationsInitGL() {
    // Initialize OpenGl settings.
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);
    gl.depthFunc(gl.LESS);
    gl.enable(gl.DEPTH_TEST);

    // Add fragment and vertex shaders to program.
    var fragShader = gl.createShader(gl.FRAGMENT_SHADER);
    var vertShader = gl.createShader(gl.VERTEX_SHADER);

    gl.shaderSource(fragShader, document.getElementById("fragShader").text);
    gl.compileShader(fragShader);
    if (!gl.getShaderParameter(fragShader, gl.COMPILE_STATUS)) {
        alert("Error while compiling fragment shader: " +
              gl.getShaderInfoLog(vertShader));
    }

    gl.shaderSource(vertShader, document.getElementById("vertShader").text);
    gl.compileShader(vertShader);
    if (!gl.getShaderParameter(vertShader, gl.COMPILE_STATUS)) {
        alert("Error while compiling vertex shader: " +
              gl.getShaderInfoLog(vertShader));
    }

    prog = gl.createProgram();
    gl.attachShader(prog, vertShader);
    gl.attachShader(prog, fragShader);
    gl.linkProgram(prog);

    if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
        alert('Error while initialising shaders.');
    }

    gl.useProgram(prog);

    // Get the location of the shader attributes and matrices.
    prog.vertexPositionAttribute = gl.getAttribLocation(prog, 'aVertexPosition');
    prog.vertexNormalAttribute = gl.getAttribLocation(prog, 'aVertexNormal');
    prog.vertexColorAttribute = gl.getAttribLocation(prog, 'aVertexColor');
    prog.cMatrixUniform = gl.getUniformLocation(prog, 'uCMatrix');
    prog.pMatrixUniform = gl.getUniformLocation(prog, 'uPMatrix');
    prog.mvMatrixUniform = gl.getUniformLocation(prog, 'uMVMatrix');
    prog.nMatrixUniform = gl.getUniformLocation(prog, 'uNMatrix');

    m4.lookAt([5.0, 5.0, 20.0], [0.0, 0.0, 0.0],[0.0, 1.0, 0.0], cMat);
    m4.inverse(cMat, cMat);

    m4.perspective(45  * (Math.PI / 180), width / height, 0.1, 100.0, pMat);


}

function TransformationsInitBuffers() {
    // Initialize vertex, normal, color and indices buffers.
    teapotVertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, teapotVertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(teapotVertices), gl.STATIC_DRAW);

    teapotNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, teapotNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(teapotNormals), gl.STATIC_DRAW);

    teapotColorBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, teapotColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(teapotColors), gl.STATIC_DRAW);

    teapotIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, teapotIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(teapotIndices), gl.STATIC_DRAW);

    // Unbind the buffers.
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
}

function TransformationsDrawGLScene() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.viewport(0, 0, width, height);

    var horizontal = Math.cos(frame * speed) * 2;
    var vertical = Math.abs(Math.sin(frame * speed) * 10) + 1;
    var rotation = Math.cos(frame * speed) * 90 * (Math.PI / 180);
    var stretch = 1 + 0.3 * Math.sin(frame * speed / 2);

    // Apply transformations.
    mvMat = m4.identity();
    if (useMyTransformations) {
        // Your transformations.
        if (doTranslate) {
            m4.multiply(mvMat, myTranslate(horizontal, vertical - 5, 0.0), mvMat);
        }
        if (doRotate) {
            m4.multiply(mvMat, myRotate(rotation, 0.2, 0.6, 0.77), mvMat);
        }
        if (doScale) {
            m4.multiply(mvMat, myScale(1 / Math.sqrt(stretch), stretch, 1 / Math.sqrt(stretch)), mvMat);
        }
    } else {
        // Library transformations.
        if (doTranslate) {
            m4.translate(mvMat, horizontal, vertical - 5, 0.0, mvMat);
        }
        if (doRotate) {
            m4.axisRotate(mvMat, [0.2, 0.6, 0.77], rotation, mvMat);
        }
        if (doScale) {
            m4.scale(mvMat, 1 / Math.sqrt(stretch), stretch, 1 / Math.sqrt(stretch), mvMat);
        }
    }

    m4.inverse(mvMat, nMat);
    m4.transpose(nMat, nMat);

    // Bind matrices and buffers to shaders.
    gl.uniformMatrix4fv(prog.cMatrixUniform, false, cMat);
    gl.uniformMatrix4fv(prog.pMatrixUniform, false, pMat);
    gl.uniformMatrix4fv(prog.mvMatrixUniform, false, mvMat);
    gl.uniformMatrix4fv(prog.nMatrixUniform, false, nMat);

    gl.bindBuffer(gl.ARRAY_BUFFER, teapotVertexBuffer);
    gl.vertexAttribPointer(prog.vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexPositionAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, teapotNormalBuffer);
    gl.vertexAttribPointer(prog.vertexNormalAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexNormalAttribute);

    gl.bindBuffer(gl.ARRAY_BUFFER, teapotColorBuffer);
    gl.vertexAttribPointer(prog.vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(prog.vertexColorAttribute);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, teapotIndexBuffer);

    // Actual drawing happens here.
    gl.drawElements(gl.TRIANGLES, teapotIndices.length, gl.UNSIGNED_SHORT, 0);

    frame++;
}

function TransformationsRenderLoop() {
    if (active == "transformations") {
        window.setTimeout(TransformationsRenderLoop, 1000/60);
        TransformationsDrawGLScene();
    }
}

function TransformationsHandleInput(event) {
    // Handle keypress events.
    switch(event.key) {
        case ' ':
            useMyTransformations = !useMyTransformations;
            break;
        case 'r':
            doRotate = !doRotate;
            break;
        case 't':
            doTranslate = !doTranslate;
            break;
        case 's':
            doScale = !doScale;
            break;
        case '+':
        case '=': // For when shift is not used.
            speed *= 1.1;
            frame = 0;
            break;
        case '-':
            speed /= 1.1;
            frame = 0;
            break;
        default:
            console.log("Not supported: " + event.key);
            break;
    }
}

function TransformationsMain() {
    const canvas = document.querySelector("#glCanvas");
    // Initialize the GL context
    gl = canvas.getContext("webgl");

    width = canvas.width;
    height = canvas.height;

    // Initialization of required variables.
    frame = 0;
    useMyTransformations = false;
    doRotate = true;
    doTranslate = true;
    doScale = true;
    speed = 0.005;

    teapotVertexBuffer = null;
    teapotNormalBuffer = null;
    teapotColorBuffer = null;
    teapotIndexBuffer = null;

    cMat = m4.identity();
    mvMat = m4.identity();
    pMat = m4.identity();
    nMat = m4.identity();

    // Only continue if WebGL is available and working
    if (!gl) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
        return;
    }

    document.onkeypress = TransformationsHandleInput;

    TransformationsInitGL();
    TransformationsInitBuffers();
    TransformationsRenderLoop();
}
