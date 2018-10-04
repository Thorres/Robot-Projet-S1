# Robot-Projet-S1

// Première chose à faire !

	--> git init

// Vérifié ce qui est modifier et aussi voir ce qui va être commit (Vert = va être commit, Rouge = ne va pas être commit).

	--> git status

// Sert à ajouter tous les dossiers dans le commirent (Les faire passer de rouge à vert).

	--> git add .

// Utilisation des branches.
	
	// Pour créer une branche.
	
		--> git checkout -b "branch_name"
		
	// Pour ajouter la branche à GitHub.
	
		--> git push origin "branch_name"
		
	// Pour passer d'une branche à l'autre.
	
		--> git checkout "branch_name"
		
	// Pour supprimer une branche.
	
		--> git push origin --delete "branch_name"

// Pour cloner le projet.

	--> git clone "git_url"

// Mettre du code sur GitHub.
	
	// Fait une sauvegarde locale du code, permet de suivre l'avancement.
	
		--> git commit -m "commit_name"
		
	// Envoyer le code sur GitHub (envoie seulement  ce qui a été commit).
	
		--> git push -u origin branch_name

// Mettre à jour le code du projet en local.

	// Vérifier s'il y a eu un changement sur GitHub. Si oui update la version locale de la branche.
	
		--> git fetch
		
	// Updater son code qui est en local pour la version la plus à jour de GitHub.
	
		--> git pull