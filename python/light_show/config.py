import json

__all__ = ("Field", "Config")


class Field:
    def __init__(self, value, description):
        self.type = type(value)
        self.value = value
        self.description = description

    def set(self, value):
        self.validate(value)
        self.value = value

    def validate(self, value):
        try:
            value = self.type(value)
        except ValueError as exc:
            raise RuntimeError(f"Unable to convert {value} to type {self.type}") from exc

    def toHtml(self, name):
        html = '<p><div class="tooltip"><span class="tooltiptext">'
        if self.type is bool:
            html += f"""{name}: <input type="radio" id="{name}" value="true" """
            html += f"""checked{'="yes"' if self.value else ''}>"""
        else:
            html += f"""{name}: <input type="text" id="{name}" value="{str(self.value)}">"""
        html += "</span></div></p>\n"
        return html


class Config(dict):
    def __init__(self, **fields):
        self._fields = fields

    def __getitem__(self, name):
        return self._fields[name]

    def __setitem__(self, name, value):
        self._fields[name].set(value)

    def __iter__(self):
        return iter(self._fields)

    def __len__(self):
        return len(self._fields)

    def update(self, **fields):
        for name, value in fields.values():
            self._fields[name].set(value)

    def write(self, filename):
        with open(filename, "wb") as fd:
            json.dump(self._fields, fd)

    @staticmethod
    def read(cls, filename):
        with open(filename, "rb") as fd:
            return cls(json.load(fd))

    def toHtml(self):
        return sum((field.toHtml(name) for name, field in self._fields.items()), "")
