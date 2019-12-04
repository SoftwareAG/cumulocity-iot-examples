FROM python:3.7-slim-buster
COPY . /app
WORKDIR /app
RUN pip install --upgrade pip
RUN pip install -r requirements
ENTRYPOINT ["python"]
CMD ["status.py"]
CMD ["service.py"]
