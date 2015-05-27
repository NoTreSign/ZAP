<h1> ZAP Robot </h1>
<h2> ZAP CONTROL INTERFACES </h2>


<p>this project file is the control system for the ZAP robot.
within this github page you will find 5 specific programs:</p>
<ol>
<li><p>The Python Code for facetracking(with thanks to Michael Mitchell (mitchtech) 
		for the facetracker code, which is used a basic for this program. </p></li>

<li><p>The Arduino Servo Controller Code. This code operates in combination with the 
		Python code trough Serial communication. for setting the servo's to a certain 
		degree type the next commands to the serial port. command structure :</p>
		<startkey><degrees><servoname><endkey>.
    #090A, or #090B, <-- this will set either servo A or B to 90 degrees.
		The # and , are start and end keys.</p></li>

<li><p>The Arduino Steppermotor Controller. This code functions in operation with the
		python code and can only be used to control the Wantai. This code listens to 
		the following command structure:</p>
    <p>if serial data == <legal command> give output.
    <p>commands are :/p
		<ul>
    <li><p>A or a :  Enable is HIGH (arduino pin D7)</p></li> 
    <li><p>B or b :  Enable is LOW (arduino pin D7)</p> </li> 
    <li><p>C or c :  Direction is HIGH (arduino pin D6) </p></li> 
    <li><p>D or d :  Direction is HIGH (arduino pin D6) </p></li> 
    <li><p>E or e :  PULSE is HIGH (arduino pin D5) <-- command for testing purposes only</p></li> 
    <li><p>F or f :  PULSE is HIGH (arduino pin D5) <-- command for testing purposes only</p></li> 
    <li><p>M or m :  Goto the middle of the frame </p></li> 
    <li><p>P or p :  Pulses (arduino pin D5): changes the output form :</p>
								 <p> HIGH -> LOW -> High (200x with delay 0.02 seconds)</p></li> 
    <li><p>R or r :  Reset moves frame to lowest position untill the microswitch is enabled.</p> 
							<p>After this the camera will move to the middle of the frame. Herefore the</p> 
							<p>frame length is measured in steps created by the pulse function. </p>
							<p>Measured hight is 35 steps.</p></li> 
	</ul></li>
<li><p>The Arduino read servo code. This programs reads multiple US en IR sensors and 
		sends back the data in a predifined protocol.
    command structure:
    <predifined name sensor><value 1 or 0>
    sensor names :UR1 , UR2 , UR3 , UR4 , UR5 , UR6 , UR7 , UR8 , UR9 , UR10 , UR11 
								  UR12 , IR1 , IR2 , IR3 , IR4</p></li>

<li><p>The simlation program. This program will readout the sensor's and idicates the 
		output values with a green or red button. This is all displayed in a GUI.</p></li>
</ol>
<p>Good luck with continuing the project.</p>
