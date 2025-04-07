import subprocess

def get_main_window_id():
    try:
        result = subprocess.run(['hyprctl', 'clients'], capture_output=True, text=True)
        
        if result.returncode != 0:
            print("Errore nell'esecuzione di hyprctl:", result.stderr)
            return None
        
        for line in result.stdout.splitlines():
            if "main" in line:  # Modifica questa condizione in base al tuo criterio per identificare la finestra principale
                parts = line.split()
                if len(parts) > 1:
                    return parts[1]  # Restituisce l'ID della finestra principale

    except Exception as e:
        print("Si è verificato un errore:", e)

    return None

if __name__ == "__main__":
    main_window_id = get_main_window_id()
    if main_window_id:
        print(main_window_id)
    else:
        print("Finestra principale non trovata.")
