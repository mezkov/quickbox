import QtQml 2.0
import qf.core 1.0
import qf.core.sql.def 1.0

Schema {
	name: 'untitled'
	tables: [
		Table { name: 'config'
			fields: [
				Field { name: 'ckey'; type: String {} },
				Field { name: 'cvalue'; type: String {} }
			]
			indexes: [
				Index {fields: ['ckey']; primary: true }
			]
		},
		Table { name: 'runners'
			fields: [
				Field { name: 'id'; type: Serial {} },
				Field { name: 'startNumber'; type: Int {} },
				Field { name: 'classId'; type: String { length: 10 } },
				Field { name: 'courseId'; type: String { length: 10 } },
				Field { name: 'firstName'; type: String {} },
				Field { name: 'lastName'; type: String {} },
				Field { name: 'registration'; type: String { length: 10 } },
				Field { name: 'licence'; type: String { length: 1 } },
				Field { name: 'club'; type: String { } },
				Field { name: 'country'; type: String { } },
				Field { name: 'offRace'; type: Boolean { } },
				Field { name: 'siId'; type: Int { } },
				Field { name: 'relayId'; type: Int { } },
				Field { name: 'leg'; type: Int { } },
				Field { name: 'note'; type: String { } },
				Field { name: 'ranking'; type: Int { } },
				Field { name: 'importId'; type: Int {} }
			]
			indexes: [
				Index {fields: ['id']; primary: true },
				Index {fields: ['classId'] },
				Index {fields: ['importId'] }
			]
		},
		Table { name: 'laps'
			fields: [
				Field { name: 'id'; type: Serial {} },
				Field { name: 'runnerId'; type: Int {} },
				Field { name: 'siId'; type: Int {} },
				Field { name: 'etap'; type: Int {} },
				Field { name: 'startTime'; type: Int {}
					comment: 'in seconds'
				},
				Field { name: 'lapTime'; type: Int {}
					comment: 'in seconds'
				},
				Field { name: 'lapTimeMS'; type: Int {}
					comment: 'in miliseconds'
				},
				Field { name: 'status';
					type: Enum { name: 'enum_lap_status';
						keys: ['NOT_RUN', 'OK', 'DISK']
					}
					defaultValue: 'NOT_RUN'; notNull: true
				}
			]
			indexes: [
				Index { fields: ['id']; primary: true },
				Index { fields: ['runnerId'] },
				Index { fields: ['etap, runnerId']; unique: true },
				Index { fields: ['status', 'lapTime', 'lapTimeMS'] }
			]
		},
		Table { name: 'etaps'
			fields: [
				Field { name: 'id'; type: Int {} },
				Field { name: 'startTime'; type: Time {} }
			]
			indexes: [
				Index {fields: ['id']; primary: true }
			]
		},
		Table { name: 'classes'
			fields: [
				Field { name: 'id'; type: String { length: 10 } },
				Field { name: 'courseId'; type: String { length: 10 } }
			]
			indexes: [
				Index {fields: ['id']; primary: true }
			]
		},
		Table { name: 'cards'
			fields: [
				Field {
					name: 'id'
					type: Serial {}
				},
				Field { name: 'stationNumber'
					type: Int {
						length: 3
						unsigned: true
					}
					//notNull: true
					defaultValue: 0
				},
				Field { name: 'idsi'
					type: Int {
						unsigned: true
					}
				},
				Field { name: 'checkTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'startTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'finishTime'
					type: Int {
						unsigned: true
					}
					comment: 'seconds in range 0 - 12 hours'
				},
				Field { name: 'punches'
					type: String {
						length: 65536
						charset: 'latin1'
					}
					comment: 'JSON of format [[code, time], ...]}'
				}
  			]
			indexes: [
				Index {
					fields: ['id']
					primary: true
				},
				Index {
					fields: ['idsi']
					unique: false
				}
			]
		}
	]
	Component.onCompleted: {
		Log.info("DbSchema created");
	}

}