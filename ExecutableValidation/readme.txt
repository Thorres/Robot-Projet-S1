Code de validation du montage de RobUS (Automne2018)




Uiliser l'outil UploadHex afin de televerser l'executabe "validation.hex" sur votre robot.

-Ouvrir un projet sur PlatFormIO(Dans l'environnement de programmation VSCode)
- Ouvrir un moniteur série.





Voici le comportement attendu de vos robot avec l’executable fournit:

- Pour les interrupteur de pare-chocs gauche et droite:
	
	- Lorsque appuye ou relache, un message correspondant devrait être lisible sur le moniteur.

- Pour l’interrupteur de pare-choc avant:
	
	- Lorsque appuye un message correspondant devrait être lisible sur le moniteur et l’avertisseur de batterie faible devrait être audible.
	
	- Lorsque relâche un message correspondant devrait être lisible sur le moniteur et l’avertisseur de batterie faible devrait s’arreter.

- Pour l’interrupteur de pare-choc arrière: 
	
	- Lorsque appuye un message correspondant devrait être lisible sur le moniteur.
	
	- Lorsque relache un message correspondant devrait être lisible sur le moniteur et le robot devrait commencer une sequence de movement.


- La sequence de movement:
	
	- Apres chaque translation et rotation, un message correspondant devrait être lisible sur le moniteur.
	
	- La sequence devrait ressembler a ceci:
		
		- Avance en ligne droite de 35 cm.
		
		- Rotation de 90 degres dans le sens antihoraire.
		
		- Avance en ligne droite de 20 cm.
		
		- Rotation de 180 degres dans le sens horaire.
		
		- Avance en ligne droite de 40 cm.
		
		- Rotation de 180 degres dans le sens horaire.
		
		- Avance en ligne droite de 20 cm.
		
		- Rotation de 90 degres dans le sens antihoraire.
		
		- Avance en ligne droite de 35 cm.
		
		
	En resume, le robot devrait tracer un “T” et revenir a sa position originale mais dans le sens inverse.
