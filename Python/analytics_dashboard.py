import time
import streamlit as st
import pandas as pd
import plotly.express as px
from pathlib import Path
from streamlit_autorefresh import st_autorefresh

CSV_FILE = Path(__file__).with_name("sensor_data1.csv")

st.set_page_config(
    page_title="Smart Environment Monitor",
    page_icon="🌍",
    layout="wide"
)
st_autorefresh(interval=5000, key="sensor_refresh")

st.title("🌍 Smart Environment Monitoring System")
st.caption("ESP32 Sensor Data Analytics Dashboard")

if not CSV_FILE.exists():
    st.error("sensor_data.csv file not found!")
    st.stop()

@st.cache_data
def load_data(file_path, modified_time):
    data = pd.read_csv(file_path, sep=",", on_bad_lines="skip")
    print("CSV columns found:", data.columns.tolist())

    print("CSV columns:", data.columns.tolist())

    data["Timestamp"] = pd.to_datetime(
        data["Timestamp"], errors="coerce"
    )

    columns = [
        "Temperature_C",
        "Humidity_Percent",
        "MQ135_Raw"
    ]

    for column in columns:
        data[column] = pd.to_numeric(
            data[column], errors="coerce"
        )

    return data.dropna(subset=["Timestamp"])


if st.button("🔄 Reload Latest Data"):
    st.cache_data.clear()

df = load_data(str(CSV_FILE), CSV_FILE.stat().st_mtime)

if df.empty:
    st.warning("CSV file mein valid readings nahi hain.")
    st.stop()

st.subheader("📊 Sensor Summary")

col1, col2, col3 = st.columns(3)

with col1:
    st.metric(
        "Average Temperature",
        f"{df['Temperature_C'].mean():.1f} °C"
    )

with col2:
    st.metric(
        "Average Humidity",
        f"{df['Humidity_Percent'].mean():.1f}%"
    )

with col3:
    st.metric(
        "Average MQ-135 Raw",
        f"{df['MQ135_Raw'].mean():.0f}"
    )

st.write(f"**Total valid readings:** {len(df)}")

st.write(
    f"**Last recorded reading:** "
    f"{df['Timestamp'].max()}"
)

st.subheader("🌡️ Temperature History")

fig_temp = px.line(
    df,
    x="Timestamp",
    y="Temperature_C",
    title="Temperature Over Time"
)
st.plotly_chart(fig_temp, use_container_width=True)

st.subheader("💧 Humidity History")

fig_humidity = px.line(
    df,
    x="Timestamp",
    y="Humidity_Percent",
    title="Humidity Over Time"
)
st.plotly_chart(fig_humidity, use_container_width=True)

st.subheader("🌫️ MQ-135 Raw Sensor History")

fig_mq = px.line(
    df,
    x="Timestamp",
    y="MQ135_Raw",
    title="MQ-135 Raw Readings Over Time"
)
st.plotly_chart(fig_mq, use_container_width=True)

st.info(
    "MQ-135 values are raw sensor readings, not calibrated AQI or ppm."
)

st.subheader("📋 Recent Sensor Readings")

st.dataframe(
    df.tail(20).sort_values("Timestamp", ascending=False),
    use_container_width=True
)

st.download_button(
    "⬇️ Download Sensor Data CSV",
    data=CSV_FILE.read_bytes(),
    file_name="sensor_data.csv",
    mime="text/csv"
)
