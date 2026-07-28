@echo off
echo Starting graph algorithm benchmark...
echo.

python -m pip show networkx >nul 2>&1
if errorlevel 1 (
    echo Installing networkx and matplotlib...
    python -m pip install networkx matplotlib
)

python orchestrator.py

echo.
echo Done! Check results\results.csv and results\live_plot.png
pause