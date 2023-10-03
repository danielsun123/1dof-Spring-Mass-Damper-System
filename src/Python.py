import plotly.graph_objects as go

t_data, x_data, v_data, a_data = [], [], [], []

with open("C:/Users/danie/Desktop/Spring_Mass_Damping_System/Spring_Mass_Damping_System_Final/data.txt", 'r') as file:
    for line in file:
        t, x, v, a = map(float, line.strip().split())
        t_data.append(t)
        x_data.append(x)
        v_data.append(v)
        a_data.append(a)

fig = go.Figure()
fig.add_trace(go.Scatter(x=t_data, y=x_data, mode='lines', name='Position (meters)'))
fig.add_trace(go.Scatter(x=t_data, y=v_data, mode='lines', name='Velocity (meters/second)'))
fig.add_trace(go.Scatter(x=t_data, y=a_data, mode='lines', name='Acceleration (meters/second^2)'))

fig.update_layout(title='Dynamic Damped Simple Harmonic Motion (SHM) Plot', xaxis_title='Time (seconds)', yaxis_title='Value')
fig.write_html('plot.html')