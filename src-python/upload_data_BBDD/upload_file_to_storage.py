from google.cloud import storage
import datetime
import os


class StorageRepository:
    """
    Clase que realiza la carga del dataframe a un bucket de Storage
    """
    _storageRepo = None
    _file_path = None

    def __init__(self, file_path='results_{}.csv'.format(str(datetime.date.today()+datetime.timedelta(days=-1)))):
        self._storageRepo = storage.Client.from_service_account_json(
            '../../storage_SA.json')
        self._file_path = file_path

    def load(self):
        """
        Metodo que carga en CoudStorage los datos de un csv indicado en la creacion de la clase.
        """
        bucket = self._storageRepo.get_bucket("resultados_diarios")
        blob_name = self._file_path
        blob = bucket.blob(blob_name)
        blob.upload_from_filename(blob_name)
        os.remove(self._file_path)


if __name__ == '__main__':
    storage = StorageRepository()
    storage.load()
